#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <jni.h>
#include <android/log.h>

#include <libevdev.h>

//#define  DEBUG
#ifdef DEBUG
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"PAPA_STICK",__VA_ARGS__)
//#define  LOGD(...)
#else
#define  LOGD(...)
#endif // DEBUG
#define  LOGE(...)  
//#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,"PAPA_STICK",__VA_ARGS__)

#define MAX_SUPPORTED_CONTACTS 10
#define VERSION 1
#define DEFAULT_SOCKET_NAME "papastick"
#define CLASS_NAME "com/papa91/gametool/libPapaStick"
#define DEFAULT_PRESSURE 50
#define EXPORT_FUNC __attribute__((visibility("default")))


void usage(const char* pname)
{
	fprintf(stderr,
		"Usage: %s [-h] [-d <device>] [-n <name>]\n"
		"  -d <device>: Use the given touch device. Otherwise autodetect.\n"
		"  -n <name>:   Change the name of of the abtract unix domain socket. (%s)\n"
		"  -h:          Show help.\n",
		pname, DEFAULT_SOCKET_NAME
		);
}

typedef struct
{
	int enabled;
	int tracking_id;
	int x;
	int y;
	int pressure;
} contact_t;

typedef struct
{
	int fd;
	int score;
	char path[100];
	struct libevdev* evdev;
	int has_mtslot;
	int has_tracking_id;
	int has_key_btn_touch;
	int has_touch_major;
	int has_width_major;
	int has_pressure;
	int min_pressure;
	int max_pressure;
	int max_x;
	int max_y;
	int max_contacts;
	int max_tracking_id;
	int tracking_id;
	contact_t contacts[MAX_SUPPORTED_CONTACTS];
} internal_state_t;

uint8_t g_bRoot = 0;
internal_state_t g_state = { 0 };
int g_socket = 0;
int g_nDomainLen = 0;

int is_character_device(const char* devpath)
{
	struct stat statbuf;

	if (stat(devpath, &statbuf) == -1) {
		perror("stat");
		return 0;
	}

	if (!S_ISCHR(statbuf.st_mode))
	{
		return 0;
	}

	return 1;
}

int is_multitouch_device(struct libevdev* evdev)
{
	return libevdev_has_event_code(evdev, EV_ABS, ABS_MT_POSITION_X);
}

int consider_device(const char* devpath, internal_state_t* state)
{
	int fd = -1;
	struct libevdev* evdev = NULL;
	int score = 1000;

	if (!is_character_device(devpath))
	{
		goto mismatch;
	}

	if ((fd = open(devpath, O_RDWR)) < 0)
	{
		perror("open");
		fprintf(stderr, "Unable to open device %s for inspection", devpath);
		LOGE("Unable to open device %s for inspection", devpath);
		goto mismatch;
	}

	if (libevdev_new_from_fd(fd, &evdev) < 0)
	{
		fprintf(stderr, "Note: device %s is not supported by libevdev\n", devpath);
		LOGE("Note: device %s is not supported by libevdev\n", devpath);
		goto mismatch;
	}

	if (!is_multitouch_device(evdev))
	{
		goto mismatch;
	}

	if (libevdev_has_event_code(evdev, EV_ABS, ABS_MT_TOOL_TYPE))
	{
		int tool_min = libevdev_get_abs_minimum(evdev, ABS_MT_TOOL_TYPE);
		int tool_max = libevdev_get_abs_maximum(evdev, ABS_MT_TOOL_TYPE);

		if (tool_min > MT_TOOL_FINGER || tool_max < MT_TOOL_FINGER)
		{
			fprintf(stderr, "Note: device %s is a touch device, but doesn't"
				" support fingers\n", devpath);
			LOGE("Note: device %s is a touch device, but doesn't"
				" support fingers\n", devpath);
			goto mismatch;
		}

		score -= tool_max - MT_TOOL_FINGER;
	}

	if (libevdev_has_event_code(evdev, EV_ABS, ABS_MT_SLOT))
	{
		score += 100;
	}

	// Alcatel OneTouch Idol 3 has an `input_mt_wrapper` device in addition
	// to direct input. It seems to be related to accessibility, as it shows
	// a touchpoint that you can move around, and then tap to activate whatever
	// is under the point. That wrapper device lacks the direct property.
	if (libevdev_has_property(evdev, INPUT_PROP_DIRECT))
	{
		score += 1000;
	}

	if (state->evdev != NULL)
	{
		if (state->score >= score)
		{
			fprintf(stderr, "Note: device %s was outscored by %s (%d >= %d)\n",
				devpath, state->path, state->score, score);
			LOGE("Note: device %s was outscored by %s (%d >= %d)\n",
				devpath, state->path, state->score, score);
			goto mismatch;
		}
		else
		{
			fprintf(stderr, "Note: device %s was outscored by %s (%d >= %d)\n",
				state->path, devpath, score, state->score);
			LOGD("Note: device %s was outscored by %s (%d >= %d)\n",
				state->path, devpath, score, state->score);
		}
	}

	libevdev_free(state->evdev);

	state->fd = fd;
	state->score = score;
	strncpy(state->path, devpath, sizeof(state->path));
	state->evdev = evdev;

	return 1;

mismatch:
	libevdev_free(evdev);

	if (fd >= 0)
	{
		close(fd);
	}

	return 0;
}

int walk_devices(const char* path, internal_state_t* state)
{
	DIR* dir;
	struct dirent* ent;
	char devpath[FILENAME_MAX];

	if ((dir = opendir(path)) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;
			}

			snprintf(devpath, FILENAME_MAX, "%s/%s", path, ent->d_name);

			LOGD("consider_device %s\n", devpath);
			consider_device(devpath, state);
		}

		closedir(dir);
		return 1;
	}
	else {
		LOGE("opendir %s error\n");
		return 0;
	}
}

int write_event(internal_state_t* state, uint16_t type,
	uint16_t code, int32_t value)
{
	// It seems that most devices do not require the event timestamps at all.
	// Left here for reference should such a situation arise.
	//
	//   timespec ts;
	//   clock_gettime(CLOCK_MONOTONIC, &ts);
	//   input_event event = {{ts.tv_sec, ts.tv_nsec / 1000}, type, code, value};

	struct input_event event = { { 0, 0 }, type, code, value };
	ssize_t result;
	ssize_t length = (ssize_t) sizeof(event);

	result = write(state->fd, &event, length);
	return result - length;
}

int next_tracking_id(internal_state_t* state)
{
	if (state->tracking_id < INT_MAX)
	{
		state->tracking_id += 1;
	}
	else
	{
		state->tracking_id = 0;
	}

	return state->tracking_id;
}

int type_a_commit(internal_state_t* state)
{
	int contact;
	int found_any = 0;

	for (contact = 0; contact < state->max_contacts; ++contact)
	{
		switch (state->contacts[contact].enabled)
		{
		case 1: // WENT_DOWN
			found_any = 1;

			if (state->has_tracking_id)
				write_event(state, EV_ABS, ABS_MT_TRACKING_ID, contact);

			if (state->has_key_btn_touch)
				write_event(state, EV_KEY, BTN_TOUCH, 1);

			if (state->has_touch_major)
				write_event(state, EV_ABS, ABS_MT_TOUCH_MAJOR, 0x00000006);

			if (state->has_width_major)
				write_event(state, EV_ABS, ABS_MT_WIDTH_MAJOR, 0x00000004);

			if (state->has_pressure)
				write_event(state, EV_ABS, ABS_MT_PRESSURE, state->contacts[contact].pressure);

			write_event(state, EV_ABS, ABS_MT_POSITION_X, state->contacts[contact].x);
			write_event(state, EV_ABS, ABS_MT_POSITION_Y, state->contacts[contact].y);

			write_event(state, EV_SYN, SYN_MT_REPORT, 0);

			state->contacts[contact].enabled = 2;
			break;
		case 2: // MOVED
			found_any = 1;

			if (state->has_tracking_id)
				write_event(state, EV_ABS, ABS_MT_TRACKING_ID, contact);

			if (state->has_touch_major)
				write_event(state, EV_ABS, ABS_MT_TOUCH_MAJOR, 0x00000006);

			if (state->has_width_major)
				write_event(state, EV_ABS, ABS_MT_WIDTH_MAJOR, 0x00000004);

			if (state->has_pressure)
				write_event(state, EV_ABS, ABS_MT_PRESSURE, state->contacts[contact].pressure);

			write_event(state, EV_ABS, ABS_MT_POSITION_X, state->contacts[contact].x);
			write_event(state, EV_ABS, ABS_MT_POSITION_Y, state->contacts[contact].y);

			write_event(state, EV_SYN, SYN_MT_REPORT, 0);
			break;
		case 3: // WENT_UP
			found_any = 1;

			if (state->has_tracking_id)
				write_event(state, EV_ABS, ABS_MT_TRACKING_ID, contact);

			if (state->has_key_btn_touch)
				write_event(state, EV_KEY, BTN_TOUCH, 0);

			write_event(state, EV_SYN, SYN_MT_REPORT, 0);

			state->contacts[contact].enabled = 0;
			break;
		}
	}

	if (found_any)
		write_event(state, EV_SYN, SYN_REPORT, 0);

	return 1;
}

int type_a_touch_panic_reset_all(internal_state_t* state)
{
	int contact;

	for (contact = 0; contact < state->max_contacts; ++contact)
	{
		switch (state->contacts[contact].enabled)
		{
		case 1: // WENT_DOWN
		case 2: // MOVED
			// Force everything to WENT_UP
			state->contacts[contact].enabled = 3;
			break;
		}
	}

	return type_a_commit(state);
}

int type_a_touch_down(internal_state_t* state, int contact, int x, int y, int pressure)
{
	if (contact >= state->max_contacts)
	{
		return 0;
	}

	if (state->contacts[contact].enabled)
	{
		type_a_touch_panic_reset_all(state);
	}

	state->contacts[contact].enabled = 1;
	state->contacts[contact].x = x;
	state->contacts[contact].y = y;
	state->contacts[contact].pressure = pressure;

	return 1;
}

int type_a_touch_move(internal_state_t* state, int contact, int x, int y, int pressure)
{
	if (contact >= state->max_contacts || !state->contacts[contact].enabled)
	{
		return 0;
	}

	state->contacts[contact].enabled = 2;
	state->contacts[contact].x = x;
	state->contacts[contact].y = y;
	state->contacts[contact].pressure = pressure;

	return 1;
}

int type_a_touch_up(internal_state_t* state, int contact)
{
	if (contact >= state->max_contacts || !state->contacts[contact].enabled)
	{
		return 0;
	}

	state->contacts[contact].enabled = 3;

	return 1;
}

int type_b_commit(internal_state_t* state)
{
	write_event(state, EV_SYN, SYN_REPORT, 0);

	return 1;
}

int type_b_touch_panic_reset_all(internal_state_t* state)
{
	int contact;
	int found_any = 0;

	for (contact = 0; contact < state->max_contacts; ++contact)
	{
		if (state->contacts[contact].enabled)
		{
			state->contacts[contact].enabled = 0;
			found_any = 1;
		}
	}

	return found_any ? type_b_commit(state) : 1;
}

int type_b_touch_down(internal_state_t* state, int contact, int x, int y, int pressure)
{
	if (contact >= state->max_contacts)
	{
		return 0;
	}

	if (state->contacts[contact].enabled)
	{
		type_b_touch_panic_reset_all(state);
	}
	//state->contacts[contact].enabled = 1;
	state->contacts[contact].tracking_id = next_tracking_id(state);

	write_event(state, EV_ABS, ABS_MT_SLOT, contact);
	write_event(state, EV_ABS, ABS_MT_TRACKING_ID,
		state->contacts[contact].tracking_id);

	if (state->has_key_btn_touch)
	{
		//int found_any = 0;
		//for (contact = 0; contact < state->max_contacts; ++contact)
		//{
		//	if (state->contacts[contact].enabled)
		//	{
		//		found_any = 1;
		//	}
		//}
		//if (found_any == 0)
		{
			write_event(state, EV_KEY, BTN_TOUCH, 1);
			LOGD("write button down\n");
		}
	}
	state->contacts[contact].enabled = 1;

	if (state->has_touch_major)
		write_event(state, EV_ABS, ABS_MT_TOUCH_MAJOR, 0x00000006);

	if (state->has_width_major)
		write_event(state, EV_ABS, ABS_MT_WIDTH_MAJOR, 0x00000004);

	if (state->has_pressure)
		write_event(state, EV_ABS, ABS_MT_PRESSURE, pressure);

	write_event(state, EV_ABS, ABS_MT_POSITION_X, x);
	write_event(state, EV_ABS, ABS_MT_POSITION_Y, y);

	return 1;
}

int type_b_touch_move(internal_state_t* state, int contact, int x, int y, int pressure)
{
	if (contact >= state->max_contacts || !state->contacts[contact].enabled)
	{
		return 0;
	}
	write_event(state, EV_ABS, ABS_MT_SLOT, contact);

	if (state->has_touch_major)
		write_event(state, EV_ABS, ABS_MT_TOUCH_MAJOR, 0x00000006);

	if (state->has_width_major)
		write_event(state, EV_ABS, ABS_MT_WIDTH_MAJOR, 0x00000004);

	if (state->has_pressure)
		write_event(state, EV_ABS, ABS_MT_PRESSURE, pressure);

	write_event(state, EV_ABS, ABS_MT_POSITION_X, x);
	write_event(state, EV_ABS, ABS_MT_POSITION_Y, y);

	return 1;
}

int type_b_touch_up(internal_state_t* state, int contact)
{
	if (contact >= state->max_contacts || !state->contacts[contact].enabled)
	{
		return 0;
	}

	state->contacts[contact].enabled = 0;

	write_event(state, EV_ABS, ABS_MT_SLOT, contact);
	write_event(state, EV_ABS, ABS_MT_TRACKING_ID, -1);

	if (state->has_key_btn_touch)
	{
		int found_any = 0;
		for (contact = 0; contact < state->max_contacts; ++contact)
		{
			if (state->contacts[contact].enabled)
			{
				found_any = 1;
			}
		}
		if (found_any == 0)
		{
			write_event(state, EV_KEY, BTN_TOUCH, 0);
			LOGD("write button up\n");
		}
	}

	return 1;
}

int touch_down(internal_state_t* state, int contact, int x, int y, int pressure)
{
	if (state->has_mtslot)
	{
		return type_b_touch_down(state, contact, x, y, pressure);
	}
	else
	{
		return type_a_touch_down(state, contact, x, y, pressure);
	}
}

int touch_move(internal_state_t* state, int contact, int x, int y, int pressure)
{
	if (state->has_mtslot)
	{
		return type_b_touch_move(state, contact, x, y, pressure);
	}
	else
	{
		return type_a_touch_move(state, contact, x, y, pressure);
	}
}

int touch_up(internal_state_t* state, int contact)
{
	if (state->has_mtslot)
	{
		return type_b_touch_up(state, contact);
	}
	else
	{
		return type_a_touch_up(state, contact);
	}
}

int touch_panic_reset_all(internal_state_t* state)
{
	if (state->has_mtslot)
	{
		return type_b_touch_panic_reset_all(state);
	}
	else
	{
		return type_a_touch_panic_reset_all(state);
	}
}

int commit(internal_state_t* state)
{
	if (state->has_mtslot)
	{
		return type_b_commit(state);
	}
	else
	{
		return type_a_commit(state);
	}
}

int start_server(char* sockname)
{
	//int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0)
	{
		perror("creating socket");
		return fd;
	}

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	//addr.sun_family = AF_UNIX;
	//strncpy(&addr.sun_path[1], sockname, strlen(sockname));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(1111);

	//if (bind(fd, (struct sockaddr*) &servaddr,
	//	sizeof(sa_family_t) + strlen(sockname) + 1) < 0)
	//{
	//	perror("binding socket");
	//	close(fd);
	//	return -1;
	//}
	int opt;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt));

	if (bind(fd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
	{
		perror("binding socket");
		close(fd);
		return -1;
	}

	listen(fd, 1);

	return fd;
}

void process_cmd(char* io_buffer, internal_state_t& state)
{
	long int contact, x, y, pressure;
	char* cursor = io_buffer;
	cursor += 1;
	switch (io_buffer[0])
	{
	case 'C': // COMMIT
		LOGD("process_cmd %s\n", io_buffer);
		commit(&state);
		break;
	case 'R': // RESET
		LOGD("process_cmd %s\n", io_buffer);
		touch_panic_reset_all(&state);
		break;
	case 'D': // TOUCH DOWN
		contact = strtol(cursor, &cursor, 10);
		x = strtol(cursor, &cursor, 10);
		y = strtol(cursor, &cursor, 10);
		pressure = strtol(cursor, &cursor, 10);
		LOGD("touch_down %ld, %ld, %ld, %ld, \n", contact, x, y, pressure);
		touch_down(&state, contact, x, y, pressure);
		break;
	case 'M': // TOUCH MOVE
		contact = strtol(cursor, &cursor, 10);
		x = strtol(cursor, &cursor, 10);
		y = strtol(cursor, &cursor, 10);
		pressure = strtol(cursor, &cursor, 10);
		LOGD("touch_move %ld, %ld, %ld, %ld, \n", contact, x, y, pressure);
		touch_move(&state, contact, x, y, pressure);
		break;
	case 'U': // TOUCH UP
		contact = strtol(cursor, &cursor, 10);
		LOGD("touch_up %ld\n", contact);
		touch_up(&state, contact);
		break;
	default:
		break;
	}
}

int main(int argc, char* argv[])
{
	const char* pname = argv[0];
	const char* devroot = "/dev/input";
	char* device = NULL;
	char* sockname = DEFAULT_SOCKET_NAME;

	int opt;
	while ((opt = getopt(argc, argv, "d:n:h")) != -1) {
		switch (opt) {
		case 'd':
			device = optarg;
			break;
		case 'n':
			sockname = optarg;
			break;
		case '?':
			usage(pname);
			return EXIT_FAILURE;
		case 'h':
			usage(pname);
			return EXIT_SUCCESS;
		}
	}

	internal_state_t state = { 0 };

	if (device != NULL)
	{
		if (!consider_device(device, &state))
		{
			fprintf(stderr, "start server failed:%s is not a supported touch device\n", device);
			return EXIT_FAILURE;
		}
	}
	else
	{
		if (!walk_devices(devroot, &state))
		{
			fprintf(stderr, "start server failed:Unable to crawl %s for touch devices\n", devroot);
			return EXIT_FAILURE;
		}
	}

	if (state.evdev == NULL)
	{
		fprintf(stderr, "start server failed:Unable to find a suitable touch device\n");
		return EXIT_FAILURE;
	}

	state.has_mtslot =
		libevdev_has_event_code(state.evdev, EV_ABS, ABS_MT_SLOT);
	state.has_tracking_id =
		libevdev_has_event_code(state.evdev, EV_ABS, ABS_MT_TRACKING_ID);
	state.has_key_btn_touch =
		libevdev_has_event_code(state.evdev, EV_KEY, BTN_TOUCH);
	state.has_touch_major =
		libevdev_has_event_code(state.evdev, EV_ABS, ABS_MT_TOUCH_MAJOR);
	state.has_width_major =
		libevdev_has_event_code(state.evdev, EV_ABS, ABS_MT_WIDTH_MAJOR);

	state.has_pressure =
		libevdev_has_event_code(state.evdev, EV_ABS, ABS_MT_PRESSURE);
	state.min_pressure = state.has_pressure ?
		libevdev_get_abs_minimum(state.evdev, ABS_MT_PRESSURE) : 0;
	state.max_pressure = state.has_pressure ?
		libevdev_get_abs_maximum(state.evdev, ABS_MT_PRESSURE) : 0;

	state.max_x = libevdev_get_abs_maximum(state.evdev, ABS_MT_POSITION_X);
	state.max_y = libevdev_get_abs_maximum(state.evdev, ABS_MT_POSITION_Y);

	state.max_tracking_id = state.has_tracking_id
		? libevdev_get_abs_maximum(state.evdev, ABS_MT_TRACKING_ID)
		: INT_MAX;

	if (!state.has_mtslot && state.max_tracking_id == 0)
	{
		// The touch device reports incorrect values. There would be no point
		// in supporting ABS_MT_TRACKING_ID at all if the maximum value was 0
		// (i.e. one contact). This happens on Lenovo Yoga Tablet B6000-F,
		// which actually seems to support ~10 contacts. So, we'll just go with
		// as many as we can and hope that the system will ignore extra contacts.
		state.max_tracking_id = MAX_SUPPORTED_CONTACTS - 1;
		fprintf(stderr,
			"Note: type A device reports a max value of 0 for ABS_MT_TRACKING_ID. "
			"This means that the device is most likely reporting incorrect "
			"information. Guessing %d.\n",
			state.max_tracking_id
			);
	}

	state.max_contacts = state.has_mtslot
		? libevdev_get_abs_maximum(state.evdev, ABS_MT_SLOT) + 1
		: (state.has_tracking_id ? state.max_tracking_id + 1 : 2);

	state.tracking_id = 0;

	int contact;
	for (contact = 0; contact < MAX_SUPPORTED_CONTACTS; ++contact)
	{
		state.contacts[contact].enabled = 0;
	}

	fprintf(stderr,
		"%s touch device %s (%dx%d with %d contacts) detected on %s (score %d)\n",
		state.has_mtslot ? "Type B" : "Type A",
		libevdev_get_name(state.evdev),
		state.max_x, state.max_y, state.max_contacts,
		state.path, state.score
		);

	if (state.max_contacts > MAX_SUPPORTED_CONTACTS) {
		fprintf(stderr, "Note: hard-limiting maximum number of contacts to %d\n",
			MAX_SUPPORTED_CONTACTS);
		state.max_contacts = MAX_SUPPORTED_CONTACTS;
	}

	struct sockaddr_un client_addr;
	socklen_t client_addr_length = sizeof(client_addr);

	int server_fd = start_server(sockname);

	if (server_fd < 0)
	{
		fprintf(stderr, "start server failed:Unable to start server on %s\n", sockname);
		return EXIT_FAILURE;
	}
	fprintf(stderr, "start server succeeded\n");
	while (1)
	{
		//int client_fd = accept(server_fd, (struct sockaddr *) &client_addr,
		//	&client_addr_length);
		int client_fd = accept(server_fd, (struct sockaddr*)NULL, NULL);

		if (client_fd < 0)
		{
			perror("accepting client");
			exit(1);
		}

		fprintf(stderr, "Connection established\n");

		char io_buffer[80] = { 0 };
		int io_length = 0;
		char* cursor;
		long int contact, x, y, pressure;

		// touch_panic_reset_all(&state);

		// Tell version
		io_length = snprintf(io_buffer, sizeof(io_buffer), "v %d\n", VERSION);
		//write(client_fd, io_buffer, io_length);
		send(client_fd, io_buffer, io_length, 0);

		// Tell limits
		io_length = snprintf(io_buffer, sizeof(io_buffer), "^ %d %d %d %d\n",
			state.max_contacts, state.max_x, state.max_y, state.max_pressure);
		//write(client_fd, io_buffer, io_length);
		send(client_fd, io_buffer, io_length, 0);

		// Tell pid
		io_length = snprintf(io_buffer, sizeof(io_buffer), "$ %d\n", getpid());
		//write(client_fd, io_buffer, io_length);
		send(client_fd, io_buffer, io_length, 0);

		while (1)
		{
			io_length = 0;

			//while (io_length < sizeof(io_buffer) &&
			//	read(client_fd, &io_buffer[io_length], 1) == 1)
			while (io_length < sizeof(io_buffer) &&
				recv(client_fd, &io_buffer[io_length], 1, 0) == 1)
			{
				if (io_buffer[io_length++] == '\n')
				{
					break;
				}
			}

			if (io_length <= 0)
			{
				break;
			}

			if (io_buffer[io_length - 1] != '\n')
			{
				continue;
			}

			if (io_length == 1)
			{
				continue;
			}

			process_cmd(io_buffer, state);
		}

		fprintf(stderr, "Connection closed\n");
		close(client_fd);
	}

	close(server_fd);
	
	/*{
		
		char cmd[80] = { 0 };
		int x = 512;
		int y = 512;
		int contact = 1;
		int io_length = snprintf(cmd, sizeof(cmd), "D %d %d %d %d\n", contact, x, y, DEFAULT_PRESSURE);
		process_cmd(cmd, g_state);
		commit(&g_state);
		
		io_length = snprintf(cmd, sizeof(cmd), "U %d\n", contact);
		process_cmd(cmd, g_state);
		commit(&g_state);
	}*/

	libevdev_free(state.evdev);
	close(state.fd);

	return EXIT_SUCCESS;
}


static jint init_device(JNIEnv *env, jobject thiz, jboolean root)
{
	const char* devroot = "/dev/input";
	char* device = NULL;
	g_bRoot = root;
	if (root)
	{
		if (!walk_devices(devroot, &g_state))
		{
			LOGE("init error");
			return 1;
		}

		if (g_state.evdev == NULL)
		{
			LOGE("cant to find a touch device\n");
			return 1;
		}

		g_state.has_mtslot =
			libevdev_has_event_code(g_state.evdev, EV_ABS, ABS_MT_SLOT);
		g_state.has_tracking_id =
			libevdev_has_event_code(g_state.evdev, EV_ABS, ABS_MT_TRACKING_ID);
		g_state.has_key_btn_touch =
			libevdev_has_event_code(g_state.evdev, EV_KEY, BTN_TOUCH);
		g_state.has_touch_major =
			libevdev_has_event_code(g_state.evdev, EV_ABS, ABS_MT_TOUCH_MAJOR);
		g_state.has_width_major =
			libevdev_has_event_code(g_state.evdev, EV_ABS, ABS_MT_WIDTH_MAJOR);

		g_state.has_pressure =
			libevdev_has_event_code(g_state.evdev, EV_ABS, ABS_MT_PRESSURE);
		g_state.min_pressure = g_state.has_pressure ?
			libevdev_get_abs_minimum(g_state.evdev, ABS_MT_PRESSURE) : 0;
		g_state.max_pressure = g_state.has_pressure ?
			libevdev_get_abs_maximum(g_state.evdev, ABS_MT_PRESSURE) : 0;

		g_state.max_x = libevdev_get_abs_maximum(g_state.evdev, ABS_MT_POSITION_X);
		g_state.max_y = libevdev_get_abs_maximum(g_state.evdev, ABS_MT_POSITION_Y);

		g_state.max_tracking_id = g_state.has_tracking_id
			? libevdev_get_abs_maximum(g_state.evdev, ABS_MT_TRACKING_ID)
			: INT_MAX;

		if (!g_state.has_mtslot && g_state.max_tracking_id == 0)
		{
			// The touch device reports incorrect values. There would be no point
			// in supporting ABS_MT_TRACKING_ID at all if the maximum value was 0
			// (i.e. one contact). This happens on Lenovo Yoga Tablet B6000-F,
			// which actually seems to support ~10 contacts. So, we'll just go with
			// as many as we can and hope that the system will ignore extra contacts.
			g_state.max_tracking_id = MAX_SUPPORTED_CONTACTS - 1;
			LOGD("type A device reports a max value of 0 for ABS_MT_TRACKING_ID. "
				"This means that the device is most likely reporting incorrect "
				"information. Guessing %d.\n",
				g_state.max_tracking_id);
		}

		g_state.max_contacts = g_state.has_mtslot
			? libevdev_get_abs_maximum(g_state.evdev, ABS_MT_SLOT) + 1
			: (g_state.has_tracking_id ? g_state.max_tracking_id + 1 : 2);

		g_state.tracking_id = 0;

		int contact;
		for (contact = 0; contact < MAX_SUPPORTED_CONTACTS; ++contact)
		{
			g_state.contacts[contact].enabled = 0;
		}

		LOGD("%s touch device %s (%dx%d with %d contacts) detected on %s (score %d)\n",
			g_state.has_mtslot ? "Type B" : "Type A",
			libevdev_get_name(g_state.evdev),
			g_state.max_x, g_state.max_y, g_state.max_contacts,
			g_state.path, g_state.score
			);

		if (g_state.max_contacts > MAX_SUPPORTED_CONTACTS) {
			LOGD("Note: hard-limiting maximum number of contacts to %d\n",
				MAX_SUPPORTED_CONTACTS);
			g_state.max_contacts = MAX_SUPPORTED_CONTACTS;
		}
	}
	else
	{//not rooted
		struct sockaddr_in addr;
		//char* sockname = DEFAULT_SOCKET_NAME;
		//g_socket = socket(AF_UNIX, SOCK_STREAM, 0);
		g_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (g_socket < 0)
		{
			LOGE("creating socket error\n");
			return 1;
		}

		memset(&addr, 0, sizeof(addr));
		//addr.sun_family = AF_UNIX;
		addr.sin_family = AF_INET; //设置为IP通信  
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(1111);

		//strncpy(&addr.sun_path[1], sockname, strlen(sockname));
		//g_nDomainLen = sizeof(sa_family_t) + strlen(sockname) + 1;
		//unlink(sockname);
		//if (bind(fd, (struct sockaddr*) &addr,
		//	sizeof(sa_family_t) + strlen(DEFAULT_SOCKET_NAME) + 1) < 0)
		//if (bind(g_socket, (struct sockaddr*) &addr, g_nDomainLen) < 0)
		//{
		//	LOGE("bind socket error len = %d\n", g_nDomainLen);
		//	close(g_socket);
		//	return 1;
		//}
		
		//if (connect(g_socket, (struct sockaddr*) &addr, g_nDomainLen) < 0)
		if (connect(g_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		{
			LOGE("connect socket error %d\n", errno);
			close(g_socket);
			return 1;
		}
	}

	return 0;
}

static void j_touch_down(JNIEnv *env, jobject thiz, jint contact, jint x, jint y)
{
	char cmd[80] = { 0 };
	int io_length = snprintf(cmd, sizeof(cmd), "D %d %d %d %d\n", contact, x, y, DEFAULT_PRESSURE);
	LOGD("%s", cmd);
	if (g_bRoot)
	{
		process_cmd(cmd, g_state);
		commit(&g_state);
	}
	else
	{
		//write(g_socket, cmd, io_length);
		send(g_socket, cmd, io_length, 0);
		io_length = snprintf(cmd, sizeof(cmd), "C\n");
		//write(g_socket, cmd, io_length);
		send(g_socket, cmd, io_length, 0);
	}
}

static void j_touch_move(JNIEnv *env, jobject thiz, jint contact, jint x, jint y)
{
	char cmd[80] = { 0 };
	int io_length = snprintf(cmd, sizeof(cmd), "M %d %d %d %d\n", contact, x, y, DEFAULT_PRESSURE);
	LOGD("%s", cmd);
	if (g_bRoot)
	{
		process_cmd(cmd, g_state);
		commit(&g_state);
	}
	else
	{
		//write(g_socket, cmd, io_length);
		send(g_socket, cmd, io_length, 0);
		io_length = snprintf(cmd, sizeof(cmd), "C\n");
		//write(g_socket, cmd, io_length);
		send(g_socket, cmd, io_length, 0);
	}
}

static void j_touch_up(JNIEnv *env, jobject thiz, jint contact)
{
	char cmd[80] = { 0 };
	int io_length = snprintf(cmd, sizeof(cmd), "U %d\n", contact);
	LOGD("%s", cmd);
	if (g_bRoot)
	{
		process_cmd(cmd, g_state);
		commit(&g_state);
	}
	else
	{
		//write(g_socket, cmd, io_length);
		send(g_socket, cmd, io_length, 0);
		io_length = snprintf(cmd, sizeof(cmd), "C\n");
		//write(g_socket, cmd, io_length);
		send(g_socket, cmd, io_length, 0);
	}
}

static void j_touch_reset()
{
	if (g_bRoot)
	{
		touch_panic_reset_all(&g_state);
	}
	else
	{
		char cmd[80] = { 0 };
		int io_length = snprintf(cmd, sizeof(cmd), "R\n");
		//write(g_socket, cmd, io_length);
		send(g_socket, cmd, io_length, 0);
	}
	
}

static JNINativeMethod methods[] = {
		{ "init_device", "(Z)I", (void*)init_device },
		{ "j_touch_down", "(III)V", (void*)j_touch_down },
		{ "j_touch_move", "(III)V", (void*)j_touch_move },
		{ "j_touch_up", "(I)V", (void*)j_touch_up },
		{ "j_touch_reset", "()V", (void*)j_touch_reset },
};

__attribute__((visibility("default"))) jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
	{
		return JNI_ERR;
	}

	jclass cls = env->FindClass(CLASS_NAME);
	if (cls == NULL)
	{
		return JNI_ERR;
	}

	int len = sizeof(methods) / sizeof(methods[0]);
	if (env->RegisterNatives(cls, methods, len) < 0)
	{
		LOGE("RegisterNatives failed!");
		return JNI_ERR;
	}

	LOGD("RegisterNatives OK!");
	return JNI_VERSION_1_4;
}
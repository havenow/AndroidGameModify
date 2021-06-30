#ifndef _mono_h
#define _mono_h

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    MONO_IMAGE_OK,
    MONO_IMAGE_ERROR_ERRNO,
    MONO_IMAGE_MISSING_ASSEMBLYREF,
    MONO_IMAGE_IMAGE_INVALID
} MonoImageOpenStatus;

//#define MonoReflectionMethod	void
#define MonoDomain 				void
#define MonoMethod				void
#define MonoClass				void
//#define MonoObject			void
#define MonoArray				void*
#define MonoVTable				void
#define MonoThreadsSync			void
#define MonoString				void
#define MonoReflectionType		void
#define MonoClassField			void
#define MonoThread				void
#define MonoImage				void
#define MonoAssembly			void

struct MonoObject
{
	MonoVTable *vtable;
	MonoThreadsSync *synchronisation;
};

struct MonoReflectionMethod 
{
	MonoObject object;
	MonoMethod *method;
	MonoString *name;
	MonoReflectionType *reftype;
};

struct WateringCan
{
	int waterCanMax;
	int waterLeft;
};

typedef MonoString* (*mono_object_to_string_t)(MonoObject *obj, MonoObject **exc);
typedef char* (*mono_string_to_utf8_t)(MonoString *string_obj);
typedef void* (*mono_object_unbox_t)(MonoObject *obj);
typedef MonoObject* (*mono_object_new_t)(MonoDomain *domain, MonoClass *klass);
typedef MonoClass* (*mono_method_get_class_t)(MonoMethod *method);
typedef MonoDomain* (*mono_domain_get_t)();
typedef MonoVTable* (*mono_class_vtable_t)(MonoDomain *domain, MonoClass *klass);
typedef MonoClassField* (*mono_class_get_field_from_name_t)(MonoClass *klass, const char *name);
typedef void (*mono_field_static_get_value_t)(MonoVTable *vt, MonoClassField *field, void *value);//public static int timeOfDay = 600; &timeOfDay
typedef void (*mono_field_static_set_value_t)(MonoVTable *vt, MonoClassField *field, void *value);
typedef void (*mono_field_get_value_t)(MonoObject *obj, MonoClassField *field, void *value) ;
typedef MonoObject* (*mono_field_get_value_object_t)(MonoDomain *domain, MonoClassField *field, MonoObject *obj);
typedef void (*mono_field_set_value_t)(MonoObject *obj, MonoClassField *field, void *value);
typedef void (*mono_runtime_object_init_t)(MonoObject *this_obj);
typedef MonoClassField* (*mono_class_get_fields_t)(MonoClass* klass, void **iter);
typedef const char* (*mono_field_get_name_t)(MonoClassField *field);
typedef uint32_t (*mono_field_get_flags_t)(MonoClassField *field);//uint32_t mono_field_get_flags(MonoClassField *field);
typedef MonoObject* (*mono_object_new_alloc_specific_t)(MonoVTable *vtable);
typedef MonoReflectionMethod* (*mono_method_get_object_t)(MonoDomain *domain, MonoMethod *method, MonoClass *refclass);
typedef void (*mono_add_internal_call_t)(const char *name, const void* method);

typedef MonoDomain* (*mono_get_root_domain_t)();
typedef MonoThread* (*mono_thread_attach_t)(MonoDomain *domain);
typedef void (*mono_thread_detach_t)(MonoThread *thread);
typedef MonoImage* (*mono_image_open_from_data_t) (char *data, uint32_t data_len, int32_t need_copy, MonoImageOpenStatus *status);
typedef MonoAssembly* (*mono_assembly_load_from_full_t)(MonoImage *image, const char*fname, MonoImageOpenStatus *status, uint32_t refonly);
typedef MonoImage* (*mono_assembly_get_image_t)(MonoAssembly *assembly);
typedef MonoClass* (*mono_class_from_name_t)(MonoImage *image, const char* name_space, const char *name);
typedef MonoMethod* (*mono_class_get_method_from_name_t)(MonoClass *klass, const char *name, int param_count);
typedef MonoObject* (*mono_runtime_invoke_t)(MonoMethod* method, void* obj, MonoArray* params, MonoObject** exc);


void cheat_init(unsigned long baseAddr);
void cheat_process(int index, int args);
void cheat_time_keep();//call it after cheat_init
void cheat_waterCanMax_keep();
void cheat_dailyLuckMax_keep();
void cheat_stamina_max_keep();
void cheat_health_max_keep();
void exit_mono();

/*
https://zhuanlan.zhihu.com/p/266597594
 * @method: method to invoke
 * @obJ: object instance					it should be NULL for static methods
 * @params: arguments to the method
 * @exc: exception information.				You can pass NULL as the exc argument if you don't want to catch exceptions
 
 Invokes the method represented by @method on the object @obj.
 obj is the 'this' pointer, it should be NULL for static
 methods, a MonoObject* for object instances and a pointer to
 the value type for value types.
 
 The params array contains the arguments to the method with the
 same convention: MonoObject* pointers for object instances and
 pointers to the value type otherwise.
 
 From unmanaged code you'll usually use the
 mono_runtime_invoke() variant.
 
 Note that this function doesn't handle virtual methods for
 you, it will exec the exact method you pass: we still need to
 expose a function to lookup the derived class implementation
 of a virtual method (there are examples of this in the code,
 though).
 
 You can pass NULL as the exc argument if you don't want to
 catch exceptions, otherwise, *exc will be set to the exception
 thrown, if any.  if an exception is thrown, you can't use the
 MonoObject* result from the function.
 
 If the method returns a value type, it is boxed in an object
 reference.
*/

/*
https://github.com/mono/mono/blob/bdd772531d379b4e78593587d15113c37edd4a64/mono/metadata/object.c
 * mono_object_new:
 * \param klass the class of the object that we want to create
 * \returns a newly created object whose definition is
 * looked up using \p klass.   This will not invoke any constructors, 
 * so the consumer of this routine has to invoke any constructors on
 * its own to initialize the object.
 * 
 * It returns NULL on failure.
 
 MonoObject *mono_object_new (MonoDomain *domain, MonoClass *klass)
 */
 
 /* 
 https://github.com/mono/mono/blob/bdd772531d379b4e78593587d15113c37edd4a64/samples/embed/test-invoke.c
 mono_object_new () doesn't call any constructor: this means that
	 * we'll have to invoke the constructor if needed ourselves. Note:
	 * invoking a constructor is no different than calling any other method,
	 * so we'll still call mono_runtime_invoke (). This also means that we 
	 * can invoke a constructor at any time, like now.
	 * First, setup the array of arguments and their values.
	 */
/* mono_object_new () only allocates the storage: 
	 * it doesn't run any constructor. Tell the runtime to run
	 * the default argumentless constructor.
	 */	 
	
/* 
sdks/wasm/src/zlib-helper.c
typedef unsigned short gushort;
typedef long           glong;
typedef unsigned long  gulong;
typedef void *         gpointer; 

mono/docs/internal-calls
https://github.com/mono/mono/blob/bdd772531d379b4e78593587d15113c37edd4a64/docs/internal-calls
	The following lists how the C# types are exposed to the C API.

	C# type 	C type
	-----------------------------
	char 		gunichar2
	bool 		MonoBoolean
	sbyte 		signed char
	byte 		guchar
	short 		gint16
	ushort 		guint16
	int 		gint32
	uint 		guint32
	long 		gint64
	ulong 		guint64
	IntPtr/UIntPtr	gpointer
	object 		MonoObject*
	string 		MonoString*
*/	
#endif

import frida
import sys
import io


device = frida.get_usb_device()

session = device.attach("com.jarworld.bleach.bvn")

scr = """
function hookTest(){
    var soAddr = Module.findBaseAddress("libCore.so");
    console.log(soAddr);

	//Java_com_adobe_air_customHandler_nativeOnTouchCallback	00506680	
	//Java_com_adobe_air_AIRWindowSurfaceView_nativeOnDoubleClickListener	005069C4	
	
	
	//Java_com_adobe_air_AIRWindowSurfaceView_nativeSetKeyboardVisible	005095D4 
	//每按一次按键会调用 堆栈 0xc6550181 base.odex!__aeabi_llsl+0x5103ad
	//0xbdfd36a3 libCore.so!Java_com_adobe_air_AIRWindowSurfaceView_nativeSetKeyboardVisible+0xce
	
	
	//0xbe4575ef libCore.so!_ZN4MMgc10EnterFrameD1Ev+0x1a
	
	
	//CorePlayer::NotifyKeyboardStateChange(int)	.text	0051304C	地址不用+1
	//CorePlayer::GenerateKeyboardAction(CorePlayer*,CorePlayer::KeyboardAutomationActionType,uint)	.text	00513084	
	//CorePlayer::GetSoftKeyboardBehavior(void)	006BE0B4	
//coreplayer::View::DispatchSoftKeyboardEvent(coreplayer::View::SoftKeyboardEventType,SoftKeyboardEventTriggerType)	005E2998	
//coreplayer::View::DispatchSoftKeyboardEvent(coreplayer::View::SoftKeyboardEventType,int,int,int,int,SoftKeyboardEventTriggerType)	005E29B2	
//avmplus::EventDispatcherObject::DispatchKeyboardEvent(avmplus::String *,bool,bool,int,int,int,bool *)	0078EAD8	
//avmplus::KeyboardEventObject::get_shiftKey(void)	00A7E1DA	

//AndroidViewClient::PlatformDismissVirtualKeyboard(void)	0051B3A6	
/*
onEnter
onLeave
0xbdfe50d1 libCore.so!_ZN17AndroidViewClient12ShowKeyboardEb+0x154
0xbdfe7767 libCore.so!_ZN13AndroidWindow10TouchEventEP14TouchEventData+0x31a
0xbdfd0805 libCore.so!Java_com_adobe_air_customHandler_nativeOnTouchCallback+0x1

----------
onEnter
onLeave
0xbdfe50d1 libCore.so!_ZN17AndroidViewClient12ShowKeyboardEb+0x154
0xbe1b3c1f libCore.so!_ZN11DisplayList14ButtonFocusSetEN4MMgc5GCAPI5GCRefI7SObje6StringEEE+0x222
0xbe0a7dd1 libCore.so!_ZN10coreplayer4View13DoButtonPointEP6SPOINTbib+0x2e4
0xbe0a83a9 libCore.so!_ZN10coreplayer4View9MouseDownEiii+0x1e4
0xbdfe76fd libCore.so!_ZN13AndroidWindow10TouchEventEP14TouchEventData+0x2b0
0xbdfd0805 libCore.so!Java_com_adobe_air_customHandler_nativeOnTouchCallback+0x1
0xc65513c3 base.odex!__aeabi_llsl+0x5115ef

onEnter
onLeave
0xbdfe50d1 libCore.so!_ZN17AndroidViewClient12ShowKeyboardEb+0x154
0xbdfe7767 libCore.so!_ZN13AndroidWindow10TouchEventEP14TouchEventData+0x31a
0xbdfd0805 libCore.so!Java_com_adobe_air_customHandler_nativeOnTouchCallback+0x1
0xc65513c3 base.odex!__aeabi_llsl+0x5115ef
*/


//DisplayList::DoButton(MMgc::GCAPI::GCRef<SObject>,bool,int,bool)	006E9DF0	
/*
onEnter
0xbe0a7d69 libCore.so!_ZN10coreplayer4View13DoButtonPointEP6SPOINTbib+0x27c
0xbe0a83a9 libCore.so!_ZN10coreplayer4View9MouseDownEiii+0x1e4
0xbdfe76fd libCore.so!_ZN13AndroidWindow10TouchEventEP14TouchEventData+0x2b0
0xbdfd0805 libCore.so!Java_com_adobe_air_customHandler_nativeOnTouchCallback+0x184
0xc65513c3 base.odex!__aeabi_llsl+0x5115ef

onLeave
onEnter
0xbe1b3111 libCore.so!_ZN11DisplayList17ButtonFocusRemoveEib+0x58
0xbe1b3a79 libCore.so!_ZN11DisplayList14ButtonFocusSetEN4MMgc5GCAPI5GCRefI7SObjectEEibNS2_IN7avmplus
6StringEEE+0x7c
0xbe0a7dd1 libCore.so!_ZN10coreplayer4View13DoButtonPointEP6SPOINTbib+0x2e4
0xbe0a83a9 libCore.so!_ZN10coreplayer4View9MouseDownEiii+0x1e4
0xbdfe76fd libCore.so!_ZN13AndroidWindow10TouchEventEP14TouchEventData+0x2b0
0xbdfd0805 libCore.so!Java_com_adobe_air_customHandler_nativeOnTouchCallback+0x184
0xc65513c3 base.odex!__aeabi_llsl+0x5115ef

onLeave
*/

//CorePlayer::SetFocusButton(MMgc::GCAPI::GCRef<SObject>)	006B659C	
//CorePlayer::GetFocusButton(void)	006EC6B8	
//DisplayList::GetKeyButton(void)	006EB830	
//CorePlayer::DoActions(bool)	.text	00673AA8	0000009C	00000018	00000000	R	.	.	.	B	T	.
/*
onLeave
onEnter
0xbe1adcd1 libCore.so!_ZN17ProcessNewActionsD2Ev+0x30
0xbe1ae9cd libCore.so!_ZN10CorePlayer11DoPlay_IdleEb+0xb5c
0xbe1b0d97 libCore.so!_ZN10CorePlayer6DoPlayEbb+0x3ea
0xbdfdb831 libCore.so!_ZN12CommonPlayer7OnTimerEv+0x164
0xbdfde669 libCore.so!_ZN14PlatformPlayer20AndroidTimerCallbackEPv+0x30
0xbdfe146d libCore.so!_Z19UnixTimeoutCallbackj+0x24
0xc6550693 base.odex!__aeabi_llsl+0x5108bf

onLeave
onEnter
0xbe1afbd9 libCore.so!_ZN10CorePlayer14DoPlay_DoFrameEv+0x4f8
0xbe1b0dc9 libCore.so!_ZN10CorePlayer6DoPlayEbb+0x41c
0xbdfdb831 libCore.so!_ZN12CommonPlayer7OnTimerEv+0x164
0xbdfde669 libCore.so!_ZN14PlatformPlayer20AndroidTimerCallbackEPv+0x30
0xbdfe146d libCore.so!_Z19UnixTimeoutCallbackj+0x24
0xc6550693 base.odex!__aeabi_llsl+0x5108bf
*/

//CorePlayer::HandleDoActions(bool)	.text	00673B44	0000002A	00000010	00000000	R	.	.	.	B	T	.
//CorePlayer::PushAction(uchar *,int,MMgc::GCAPI::GCRef<ScriptThread>,int)	.text	006E8D50	000000D6	00000090	00000004	R	.	.	.	B	.	.

//CommonPlayer::GetKeyState(int)	.text	00511AE4	00000010			R	.	.	.	.	T	.

/*	
Java_com_adobe_air_customHandler_callTimeoutFunction	0x00506518	base.odex!__aeabi_llsl+0x5be8c1 	libc.so!je_free+0x48
Java_com_adobe_air_customHandler_nativeOnKeyCallback	0x00506528	没用
Java_com_adobe_air_customHandler_nativeOnTouchCallback	0x00506680	
Java_com_adobe_air_AIRWindowSurfaceView_nativeOnKeyListener		0x00506C80	没用
Java_com_adobe_air_AIRWindowSurfaceView_nativeGetTextBoxBounds		0x00507768	0xcfdc130d base.odex!__aeabi_llsl+0x5be539	0xc82175ef libCore.so!_ZN4MMgc10EnterFrameD1Ev+0x1a
Java_com_adobe_air_AIRWindowSurfaceView_nativeSetKeyboardVisible			0x005095D4							
*/

//CommonPlayer::HandleCustomMenuItem(int)	.text	00512B78	00000002			R	.	.	.	.	T	.
//CommonPlayer::OnMenuCommand(int)	.text	005127B4	000003AE	00000098	00000000	R	.	.	.	B	T	.
//Menu::SetScriptObject(MMgc::GCAPI::GCRef<avmplus::MenuObject>)	.text	00A7A1F8	00000006			R	.	.	.	.	.	.
//avmplus::MenuItemObject::set_data(int)	.text	007A4A9E	000000B6	00000018	00000000	R	.	.	.	B	T	.
//runtime::ContentPlayerObject::set_menu(avmplus::MenuObject *)	.text	008A803C	00000030	00000010	00000000	R	.	.	.	B	T	.
//avmplus::WindowObject::set__menu(avmplus::MenuObject *)	.text	0086E5E2	00000032	00000010	00000000	R	.	.	.	B	T	.
//PlayerDebugger::SetOption(char *,char *)	.text	00654278	00000238	00000038	00000000	R	.	.	.	B	T	.
//PlayerDebugger::OnSetOption(char *,char *)	.text	00650E04	0000002E	00000018	00000000	R	.	.	.	B	T	.
//CoreGlobals::ReadBoolProperty(char const*,char const*,ulong)	.text	0052C9F0	00000048	00000010	00000000	R	.	.	.	B	T	.
//CoreGlobals::ReadIntProperty(char const*,char const*,ulong)	.text	0052C994	00000052	00000010	FFFFFFF8	R	.	.	.	.	T	.
//CoreGlobals::ReadSettings(PlatformFileManager &,FlashFileString const&)	.text	0052CB28	00000198	00000050	00000000	R	.	.	.	B	T	.

/*
CoreGlobals::SetDevicePowerState(CoreGlobals::DevicePowerState_t,PlatformPlayer *)	.text	0x0052F42C
PlatformGlobals::SetPlatformDevicePowerState(CoreGlobals::DevicePowerState_t,PlatformPlayer *)	.text	0x00575660 			no +1 ;crash
*/

//ConfigManager::SavePersistentData(char const*,uchar const*,uint)	.text	0052ADEC	00000096	00000048	00000000	R	.	.	.	B	T	.
//ConfigManager::VerifyAndSaveConfigFile(uchar const*,uint)	.text	0052B090	00000114	00000078	00000000	R	.	.	.	B	T	.
//sqlite3Savepoint	.text	009C787C	000000E4	00000028	00000000	R	.	.	.	B	.	.

	//CorePlayer::IsRootPlayer(void)	.text	006BA068	0000001A	00000008	00000000	R	.	.	.	.	.	.
	//CorePlayer::GetRootPlayer(void)	.text	006BE234	00000010	00000008	00000000	R	.	.	.	.	.	.
	//avmplus::NativeID::flash_net_FileReference_private__ensureIsRootPlayer_thunk(avmplus::MethodEnv *,uint,int *)	.text	007D1B78	00000022			R	.	.	.	.	.	.
	//avmplus::NativeID::flash_net_FileReference_private__ensureIsRootPlayer_sampler_thunk(avmplus::MethodEnv *,uint,int *)	.text	007D1B9C	0000008C	00000028	00000000	R	.	.	.	B	.	.
	//avmplus::NativeID::flash_desktop_NativeApplication_private__isRootPlayer_thunk(avmplus::MethodEnv *,uint,int *)	.text	007E45EC	0000000C	00000008	FFFFFFF8	R	.	.	.	.	.	.
	//avmplus::NativeID::flash_desktop_NativeApplication_private__isRootPlayer_sampler_thunk(avmplus::MethodEnv *,uint,int *)	.text	007E45F8	0000007E	00000028	00000000	R	.	.	.	B	.	.
	//avmplus::WindowClass::_isRootPlayer(void)	.text	0086D6EA	00000016	00000008	FFFFFFF8	R	.	.	.	.	.	.
//CorePlayer::GetFirstDisplayList(void)	.text	006E6F08	00000036	00000020	FFFFFFF8	R	.	.	.	.	T	.
//CorePlayer::GetStageOwner(void)	.text	00706026	0000002C	00000020	FFFFFFF8	R	.	.	.	.	T	.

    var funcAddr = soAddr.add(0x00706026 +1); //函数地址计算 thumb+1 ARM不加	
    console.log(funcAddr);

    if(funcAddr != null){
		console.log("attach");
        Interceptor.attach(funcAddr,{
		
		
            onEnter: function(args){
				console.log("onEnter---------------------------------<");
				//console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
				console.log(args[0]);
            },
            onLeave: function(retval){
				//if (retval != 0)
                //console.log("onLeave---------------------------------");
				//console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
				console.log(retval);
				console.log("onLeave--------------------------------->\\n");
            }
        });
     }
}

function hookTest32(){
    var helloAddr = Module.findExportByName("libCore.so", "malloc");
    console.log("libCore.so malloc addr:	" + helloAddr);
    if(helloAddr != null){
        Interceptor.attach(helloAddr,{
            onEnter: function(args){
				//console.log("---begin")
                //console.log(args[0]);
				if (args[0] == 0x1008)//64:0x1408 32:0x1008
				{
					console.log("---begin");
					console.log(args[0]);
					//console.log(Thread.backtrace(this.context, Backtracer.ACCURATE).map(DebugSymbol.fromAddress).join('\\n') + '\\n');
					this.argsTmp = args[0];
				}
				
				/*var ios = new File("/data/local/tmp/dadnme.txt", "w");
				ios.write("--------------------------------------------------------------------begin!!!\\n");
				ios.write(Thread.backtrace(this.context, Backtracer.ACCURATE)
				.map(DebugSymbol.fromAddress).join('\\n') + '\\n');
				ios.flush();
				ios.close();*/
            },
            onLeave: function(retval){
				if(this.argsTmp == 0x1008)//64:0x1408 32:0x1008
				{
					console.log(retval);
					
					var addr1 = 0xF44;
					var addr2 = 0xAA4;
					
					console.log("---blood value")
					var strByte = retval.add(addr1).readByteArray(4);//读内存 4字节
					console.log(strByte);
					//console.log(hexdump(retval.add(addr1)));//dump指定内存
					
					console.log("---crazy value")
					var strByte = retval.add(addr2).readByteArray(4);//读内存 4字节
					console.log(strByte);
					//console.log(hexdump(retval.add(addr2)));//dump指定内存
					
					
					console.log("---end  \\n\\n")
				}
                //console.log(retval);
				//console.log("---end  \\n\\n")
            }
        });
    } 
}


function hookTest1(){
    var helloAddr = Module.findExportByName("libCocos.so", "_ZN8uirender21PottyRacer34Optimizer8saveSaveEv");
    console.log(helloAddr);
    if(helloAddr != null){
        Interceptor.attach(helloAddr,{
            onEnter: function(args){
				console.log("--------------------------------------------------------------------begin!!!")
                
            },
            onLeave: function(retval){
				console.log("--------------------------------------------------------------------end  !!!\\n\\n")
            }
        });
    } 
}

function hookSys(){
	var exports = Module.enumerateExports("libCocos.so");
	
	console.log('sys----------------');
	/*
	for(var i = 0; i < exports.length; i++){
		console.log(JSON.stringify(exports[i]));
		console.log(exports[i].address);
	}
	*/
	/*
	for(var i = 0; i < 7; i++){
		var addr = exports[i].address;
		if(addr != null){
        Interceptor.attach(addr,{
            onEnter: function(args){
				console.log("--------------------------------------------------------------------begin!!!")
                
            },
            onLeave: function(retval){
				console.log("--------------------------------------------------------------------end  !!!\\n\\n")
            }
        });
		} 
	}
	*/
	/*
	for(var i = 0; i < imports.length; i++){
		 Interceptor.attach(imports[i].address,{
            onEnter: function(){
				console.log("sys enter!!!")
                console.log(JSON.stringify(imports[i]));
            },
        });
		
	}
	*/
}

function main(){
	hookTest();
}

setImmediate(main);
"""

def on_message(message ,data):
   print('on_message')


script = session.create_script(scr)
script.on("message" , on_message)
script.load()
sys.stdin.read()
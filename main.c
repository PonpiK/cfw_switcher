#include <pspsdk.h>
#include <pspctrl.h>
#include <psptypes.h>
#include <pspiofilemgr.h>
#include <stdio.h>


// Ummm yep some of this was just stripped out of ARK-4 stuff which works well. Enjoy I guess \o/.

PSP_MODULE_INFO("CFW Switcher", 0x0800, 1, 0);

#define printf pspDebugScreenPrintf


void open_flash(){
    while(sceIoUnassign("flash0:") < 0) {
        sceKernelDelayThread(500000);
    }
    while (sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", 0, NULL, 0)<0){
        sceKernelDelayThread(500000);
    }
}


SceCtrlData pad;

int main(int argc, char *args[]) {

	pspDebugScreenInit();

	SceUID step = 1;
	int running = 1;
	u32 prev = 0;

	pspDebugScreenSetTextColor(0xAAFFFFFF);
	open_flash();
	int me_detection = sceIoOpen("flash0:/kd/pulsar.prx", PSP_O_RDONLY, 0);
	int pro_detection = sceIoOpen("flash0:/kd/_galaxy.prx", PSP_O_RDONLY, 0);
	int ark_detection = sceIoOpen("flash0:/kd/ark_systemctrl.prx", PSP_O_RDONLY, 0);

	if((me_detection && pro_detection && ark_detection) < 0) {
		printf("NO MODULES FOUND..... EXITING.....");
		sceKernelDelayThread(4*1000*1000);
		sceKernelExitGame();
	}
	 


	while(running) {
		sceDisplayWaitVblankStart();
		sceCtrlPeekBufferPositive(&pad, 1);
		if(step == 1) {
			printf("Choose your boot method\n");
			if(pro_detection >= 0)
				printf("-> PRO\n");
			if(me_detection >= 0)
				printf("   ME\n");
			if(ark_detection >= 0) 
				printf("   ARK-4\n");
			if(pro_detection < 0)
				step++;
		}
		else if(step == 2) {
			printf("Choose your boot method\n");
			if(pro_detection >= 0)
				printf("   PRO\n");
			if(me_detection >= 0)
				printf("-> ME\n");
			if(ark_detection >= 0) 
				printf("   ARK-4\n");
			if(me_detection < 0)
				step++;
		}
		else if(step == 3){
			printf("Choose your boot method\n");
			if(pro_detection >= 0)
				printf("   PRO\n");
			if(me_detection >= 0)
				printf("   ME\n");
			if(ark_detection >= 0) 
				printf("-> ARK-4\n");
			if(ark_detection < 0)
				step++;
		}
		if((pad.Buttons & PSP_CTRL_DOWN) && !(prev & PSP_CTRL_DOWN)) {
			step++;
		}
		if((pad.Buttons & PSP_CTRL_UP) && !(prev & PSP_CTRL_UP)) {
			step--;
		}

		if(pad.Buttons == PSP_CTRL_CROSS) {
			sceIoClose(me_detection);
			sceIoClose(pro_detection);
			sceIoClose(ark_detection);
			if(step == 1) {
				int fd = sceIoOpen("flash0:/arkcipl.cfg", PSP_O_WRONLY|PSP_O_CREAT|PSP_O_TRUNC, 0777);
				char config[] = "cfw=pro";
				sceIoWrite(fd, config, strlen(config));
				sceIoClose(fd);
			}
			else if(step == 2) {
				int fd = sceIoOpen("flash0:/arkcipl.cfg", PSP_O_WRONLY|PSP_O_CREAT|PSP_O_TRUNC, 0777);
				char config[] = "cfw=me";
				sceIoWrite(fd, config, strlen(config));
				sceIoClose(fd);
			}
			else {
				sceIoRemove("flash0:/arkcipl.cfg");
			}
			scePowerRequestColdReset(0);
		}

			
		
		if(step<1) step = 3;
		if(step>3) step = 1;
		
		prev = pad.Buttons;
		sceKernelDelayThread(10000);
		pspDebugScreenClear();
	}

	sceKernelExitGame();


	return 0;
}


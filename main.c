#include <string.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspiofilemgr.h>

PSP_MODULE_INFO("cfw_switcher", 0x0800, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);

typedef struct
{
	const char *name;
	const char *path;
} CFirmware;

CFirmware options[] = {
	{"ME", "flash0:/kd/pulsar.prx"},
	{"PRO", "flash0:/kd/_galaxy.prx"},
	{"ARK-4", "flash0:/kd/ark_systemctrl.prx"}};

void assignFlash(void)
{
	if (sceIoUnassign("flash0:") < 0 || sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", 0, NULL, 0) < 0)
	{
		pspDebugScreenPrintf("Failed to open flash0. Exiting in 3 seconds...\n");
		sceKernelDelayThread(3000000);
		sceKernelExitGame();
	}
}

int findModule(const char *path)
{
	int file = sceIoOpen(path, PSP_O_RDONLY, 0);
	if (file >= 0)
		sceIoClose(file);
	return file >= 0;
}

void writeConfig(const char *config)
{
	int file = sceIoOpen("flash0:/arkcipl.cfg", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (file >= 0)
	{
		sceIoWrite(file, config, strlen(config));
		sceIoClose(file);
	}
}

void drawMenu(int index, const CFirmware **list, int count)
{
	pspDebugScreenClear();
	pspDebugScreenPrintf("Choose the CFW to boot:\n\n");
	for (int i = 0; i < count; i++)
		pspDebugScreenPrintf(i == index ? "> %s\n" : "  %s\n", list[i]->name);
}

int main(void)
{
	pspDebugScreenInit();
	pspDebugScreenSetXY(0, 0);
	pspDebugScreenSetBackColor(0x00000000);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenClear();
	assignFlash();

	int count = 0, index = 0;
	const int value = sizeof(options) / sizeof(options[0]);
	const CFirmware *list[value];

	for (int i = 0; i < value; ++i)
		if (findModule(options[i].path))
			list[count++] = &options[i];

	if (count == 0)
	{
		pspDebugScreenPrintf("No CFW modules found. Exiting in 3 seconds...\n");
		sceKernelDelayThread(3000000);
		sceKernelExitGame();
	}

	drawMenu(index, list, count);
	while (1)
	{
		SceCtrlData pad;
		sceCtrlReadBufferPositive(&pad, 1);
		if (pad.Buttons & PSP_CTRL_DOWN)
		{
			index = (index + 1) % count;
			drawMenu(index, list, count);
			sceKernelDelayThread(150000);
		}
		else if (pad.Buttons & PSP_CTRL_UP)
		{
			index = (index - 1 + count) % count;
			drawMenu(index, list, count);
			sceKernelDelayThread(150000);
		}
		else if (pad.Buttons & PSP_CTRL_CROSS)
		{
			const char *cfw = list[index]->name;
			if (strcmp(cfw, "ME") == 0)
				writeConfig("cfw=me");
			else if (strcmp(cfw, "PRO") == 0)
				writeConfig("cfw=pro");
			else if (strcmp(cfw, "ARK-4") == 0)
				sceIoRemove("flash0:/arkcipl.cfg");
			else
			{
				pspDebugScreenPrintf("\nInvalid CFW selected. Exiting in 3 seconds...\n");
				sceKernelDelayThread(3000000);
				sceKernelExitGame();
			}

			pspDebugScreenPrintf("\nSwitching to %s...\n", cfw);
			sceKernelDelayThread(1000000);
			scePowerRequestColdReset(0);
		}

		sceDisplayWaitVblankStart();
	}

	return 0;
}
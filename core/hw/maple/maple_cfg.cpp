#include "types.h"
#include "maple_if.h"
#include "maple_helper.h"
#include "maple_devs.h"
#include "maple_cfg.h"

#define HAS_VMU
/*
bus_x=0{p0=1{config};p1=2{config};config;}
Plugins:
	Input Source
		EventMap -- 'Raw' interface, source_name[seid]:mode
		KeyMap -- translated chars ( no re-mapping possible)
	Output
		Image
		
*/
/*
	MapleConfig:
		InputUpdate(&fmt);
		ImageUpdate(data);
*/
void UpdateInputState(u32 port);
void UpdateVibration(u32 port, u32 value);

extern u16 kcode[4];
extern u32 vks[4];
extern s8 joyx[4],joyy[4];
extern u8 rt[4],lt[4];
extern bool enable_purupuru;

u8 GetBtFromSgn(s8 val)
{
	return val+128;
}

struct MapleConfigMap : IMapleConfigMap
{
	maple_device* dev;

	MapleConfigMap(maple_device* dev)
	{
		this->dev=dev;
	}

   void SetVibration(u32 value)
   {
      UpdateVibration(dev->bus_id, value);
   }

	void GetInput(PlainJoystickState* pjs)
	{
		UpdateInputState(dev->bus_id);

		pjs->kcode=kcode[dev->bus_id] | 0xF901;
		pjs->joy[PJAI_X1]=GetBtFromSgn(joyx[dev->bus_id]);
		pjs->joy[PJAI_Y1]=GetBtFromSgn(joyy[dev->bus_id]);
		pjs->trigger[PJTI_R]=rt[dev->bus_id];
		pjs->trigger[PJTI_L]=lt[dev->bus_id];
	}
	void SetImage(void* img)
	{
		//?
	}
};

void mcfg_Create(MapleDeviceType type,u32 bus,u32 port)
{
	maple_device* dev=maple_Create(type);
	dev->Setup(maple_GetAddress(bus,port));
	dev->config = new MapleConfigMap(dev);
	dev->OnSetup();
	MapleDevices[bus][port]=dev;
}

void mcfg_CreateDevices()
{
#if DC_PLATFORM == DC_PLATFORM_DREAMCAST
   int bus;
   for (bus = 0; bus < 4; ++bus)
   {
      mcfg_Create(MDT_SegaController,bus,5);

      mcfg_Create(MDT_SegaVMU,bus,0);

      if (enable_purupuru)
         mcfg_Create(MDT_PurupuruPack,bus,1);
      else
         mcfg_Create(MDT_SegaVMU,bus,1);
   }
#else
	mcfg_Create(MDT_NaomiJamma, 0, 5);
#endif
}

void mcfg_DestroyDevices()
{
	for (int i=0;i<=3;i++)
		for (int j=0;j<=5;j++)
			delete MapleDevices[i][j];
}

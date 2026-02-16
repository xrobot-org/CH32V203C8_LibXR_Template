#include "cdc_uart.hpp"
#include "ch32_gpio.hpp"
#include "ch32_timebase.hpp"
#include "ch32_uart.hpp"
#include "ch32_usb_dev.hpp"
#include "libxr.hpp"
#include "thread.hpp"

static uint8_t devfs_ep0_buffer[8];
static uint8_t devfs_ep1_buffer[128];
static uint8_t devfs_ep2_buffer[16];
static uint8_t otgfs_ep0_buffer[8];
static uint8_t otgfs_ep1_buffer[256];
static uint8_t otgfs_ep2_buffer[16];

extern "C" void app_main()
{
  LibXR::CH32Timebase timebase;
  LibXR::PlatformInit(3, 1024);

  LibXR::CH32GPIO led(GPIOB, GPIO_Pin_2);
  static uint8_t usart1_buffer[2][128];
  LibXR::CH32UART usart1(CH32_USART1, usart1_buffer[0], usart1_buffer[1], GPIOA,
                         GPIO_Pin_9, GPIOA, GPIO_Pin_10);

  static constexpr auto LANG_PACK_EN_US_DEVFS = LibXR::USB::DescriptorStrings::MakeLanguagePack(
      LibXR::USB::DescriptorStrings::Language::EN_US, "XRobot", "CDC DEVFS",
      "XRUSB-DEVFS-");
  static constexpr auto LANG_PACK_EN_US_OTGFS = LibXR::USB::DescriptorStrings::MakeLanguagePack(
      LibXR::USB::DescriptorStrings::Language::EN_US, "XRobot", "CDC OTGFS",
      "XRUSB-OTGFS-");

  static const LibXR::ConstRawData UID = {reinterpret_cast<void*>(0x1FFFF7E8), 12};

  static LibXR::USB::CDCUart cdc_devfs;
  static LibXR::CH32USBDeviceFS usb_devfs(
      {{devfs_ep0_buffer}, {devfs_ep1_buffer}, {devfs_ep2_buffer}},
      LibXR::USB::DeviceDescriptor::PacketSize0::SIZE_8, 0x1209, 0x0201, 0x0100,
      {&LANG_PACK_EN_US_DEVFS}, {{&cdc_devfs}}, UID);

  static LibXR::USB::CDCUart cdc_otgfs;
  static LibXR::CH32USBOtgFS usb_otgfs(
      {{otgfs_ep0_buffer}, {otgfs_ep1_buffer}, {otgfs_ep2_buffer}},
      LibXR::USB::DeviceDescriptor::PacketSize0::SIZE_8, 0x1209, 0x0202, 0x0100,
      {&LANG_PACK_EN_US_OTGFS}, {{&cdc_otgfs}}, UID);

  usb_devfs.Init(false);
  usb_devfs.Start(false);
  usb_otgfs.Init(false);
  usb_otgfs.Start(false);
  LibXR::STDIO::read_ = cdc_devfs.read_port_;
  LibXR::STDIO::write_ = cdc_devfs.write_port_;

  while (true)
  {
    led.Write(true);
    LibXR::Thread::Sleep(200);
    led.Write(false);
    LibXR::Thread::Sleep(200);
  }
}

#include <cmath>

#include "cdc_test.hpp"
#include "cdc_uart.hpp"
#include "ch32_gpio.hpp"
#include "ch32_timebase.hpp"
#include "ch32_uart.hpp"
#include "ch32_usb.hpp"
#include "ch32_usb_dev.hpp"
#include "hid_gamepad.hpp"
#include "hid_keyboard.hpp"
#include "libxr.hpp"
#include "semaphore.hpp"
#include "thread.hpp"

static uint8_t ep0_buffer[8];
static uint8_t ep1_buffer[256], ep2_buffer[16];

extern "C" void app_main()
{
  LibXR::CH32Timebase timebase;

  LibXR::PlatformInit(3, 1024);

  LibXR::CH32GPIO led(GPIOB, GPIO_Pin_2);

  static constexpr auto LANG_PACK_EN_US = LibXR::USB::DescriptorStrings::MakeLanguagePack(
      LibXR::USB::DescriptorStrings::Language::EN_US, "XRobot", "CDC Demo", "123456789");

  LibXR::USB::CDCUart cdc;

  LibXR::CH32USBDeviceFS usb_dev(
      /* EP */
      {
          {ep0_buffer},
          {ep1_buffer},
          {ep2_buffer},
      },
      /* packet size */
      LibXR::USB::DeviceDescriptor::PacketSize0::SIZE_8,
      /* vid pid bcd */
      0x1209, 0x0001, 0x0100,
      /* language */
      {&LANG_PACK_EN_US},
      /* config */
      {{&cdc}});

  usb_dev.Init();

  usb_dev.Start();

  static uint8_t usart1_buffer[2][128];

  LibXR::CH32UART usart1(CH32_USART1, usart1_buffer[0], usart1_buffer[1], GPIOA,
                         GPIO_Pin_9, GPIOA, GPIO_Pin_10);

  LibXR::STDIO::read_ = cdc.read_port_;
  LibXR::STDIO::write_ = cdc.write_port_;

  while (true)
  {
    LibXR::Thread::Sleep(UINT32_MAX);
  }
}

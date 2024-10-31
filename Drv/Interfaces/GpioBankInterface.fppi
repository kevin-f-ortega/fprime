
@ Port used to write to a GPIO pin
sync input port gpioWrite: Drv.GpioWrite

@ Port used to read from a GPIO pin
sync input port gpioRead: Drv.GpioRead

@ Port used to write to a GPIO bank
sync input port gpioBankWrite: Drv.GpioBankWrite

@ Port used to read from a GPIO bank
sync input port gpioBankRead: Drv.GpioBankRead

@ Port used to indicate transition on the GPIO pin
output port gpioInterrupt: Svc.Cycle

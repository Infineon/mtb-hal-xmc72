# Hardware Abstraction Layer

## Overview

Hardware Abstraction Layer (HAL) provides a generic, device agnostic interface that can be used across multiple product families. The HAL is designed for portability, not as a general-purpose user library. HAL drivers are thin wrappers around lower-level device specific (e.g.: PDL) APIs and are focused on the runtime behavior of the hardware, not initialization.

HAL drivers are independent of each other and they do not provide system level configurations. Not all HAL drivers are applicable for all devices, because some do not have the necessary capabilities to support a feature/driver. When a HAL driver is not available for a device, `MTB_HAL_DRIVER_AVAILABLE_` macro for the driver will be set to false. When a function feature is not available in a driver, the function returns an unsupported result code. The HAL is designed to be compatible with a wide range of devices including low memory, multi-core, connectivity and secure devices.

Middleware use the HAL APIs to support portability across multiple device variants. Users/application code may also use the HAL API, if the intent of the application or the solution code example is to be portable across multiple devices.

## API Structure
The API functions for each HAL driver can be divided into the following categories:
* A `_setup` function that sets up the HAL object with the pre-initialized hardware
* Functions to support run-time operations.

The `_setup` API functions require a pointer to an instance of a driver-specific type (e.g. `mtb_hal_uart_t`). The instance must be allocated by the application code and passed via a pointer into the setup function, which will populate its content. This approach allows the HAL to avoid performing dynamic memory allocations. From an application/middleware point of view, these structs are opaque object handles. The same object must be passed to all subsequent API calls that operate on the same hardware instance. Application should pass this HAL object as a pointer to the libraries using the HAL interface. The HAL object struct definitions are defined within the platform-specific HAL implementation. Application/Middleware code should not rely on the specific contents, which is an internal implementation detail and is subject to change between platforms and/or HAL releases.

The `_setup` functions also require a pointer to the HAL configuration structure `_hal_config`. This structure is generated by the device configurator tool during application project build. The tool generates c-code that include the hardware configuration parameters needed for HAL operations. For drivers that allow for clock modifications, the structure may also reference a pointer to a `mtb_hal_clock_t` object, which includes the default clock interface functions implemented in the HAL.

Many `_setup` functions also include an argument pointing to `mtb_hal_clock_t` instance, if applicable. This is an optional argument to enable the application to setup the clock interface functions. An example use case for this functionality is where the application sets up the clock interface solely within a security partition responsible for managing the clock, while the peripheral itself is controlled within a non-secure partition. The separation of responsibility for resource management allows for clock reconfiguration to be handled in a different domain from the rest of the peripheral activities. If a `NULL` value is passed in the `_setup` function, it will use the default clock interface functions implemented in the HAL that is available as part of the `_hal_config` structure.

## Initialization Sequence
The application should initialize all hardware intended to be accessed through HAL using the PDL APIs. The initialization sequence should be as follows.
* Initialize the driver using the PDL APIs.
* Setup the HAL object that corresponds to the driver using the `_setup` function.
* Enable the driver using the PDL APIs.
* Configure and enable the interrupt handler, if applicable, using PDL APIs.
* Register the syspm callback handler, if any, using PDL APIs

If the device configurator is used to configure the hardware, the tool automatically generates the structures that can be used for configuration purposes. The application is free to ignore the configurator generated structures and define the structures by hand if necessary.

## Interrupt Handling
HAL drivers that support interrupt handling expose a `_process_interrupt` function to perform peripheral interrupt processing. Middleware that uses a HAL driver must wrap that HAL driver's `_process_interrupt` function(s) with a middleware-level interrupt processing function. For each peripheral instance that is used by a middleware, the application is responsible for configuring and registering an interrupt handler which invokes the corresponding middleware interrupt processing function.

## Event Handling
Many HAL drivers provide an API for registering a callback which is invoked when certain (driver-specific) events occur. These drivers also often provide an API for enabling or disabling specific types of events. Unless otherwise documented, the callback will only be invoked for events that occur while that event type is enabled. Specifically, events that occur while a given event type is disabled are not queued and will not trigger a callback when that event type is (re)enabled.

## Clock Management
Application is responsible for configuring and initializing the clock. For situations where the HAL needs to modify the clock frequency at runtime (e.g. if UART baud rate needs to be changed), the HAL `_setup` accepts a `mtb_hal_clock_t` argument. This argument could be passed to the `_setup` function from either of the two sources. The first is the default HAL clock interface and the second is the application specific clock interface.
* As part of the configurator generated structure `_hal_config`:
HAL provides a default interface for setting up clock dividers and the configurator generated structure `_hal_config` includes a `mtb_hal_clock_t` type pointer that points to this default interface. This ensures that the user does not have to do anything special in simple use cases.
* As an argument to the `_setup` function:
 In case the application needs to provide a custom implementation of the clock interface (e.g. due to the device security features) `_setup` function accepts a pointer to `mtb_hal_clock_t` type that points to the application specific clock interface functions. An example use case where the application utilizing this argument could involve the application setting up the clock interface solely within a security partition responsible for managing the clock. If the application desires to the use the HAL default clock interface (case #1), this argument can be passed as `NULL`.

## Compile Time Dependencies
The HAL public interface is consistent across all platforms that the HAL supports. However, the HAL interface does depend on types that are defined by platform-specific HAL implementations (e.g. the driver-specific "handle" types). Additionally, a HAL implementation may be compile-time dependent on device-specific data structures. This means that in order to compile an application that includes a middleware that depends on the HAL, it must build against a device support library that includes the HAL.

## Error reporting
The HAL uses the `cy_rslt_t` type (from the core-lib library) in all places where a HAL function might return an error code. This provides structured error reporting and makes it easy to determine the module from which the error arose, as well as the specific error cause. Macros are provided to help extract this information from a `cy_rslt_t` value.

The HAL driver headers provide macros corresponding to codes for common error situations. Note that the errors listed in the driver header are not a comprehensive list; in less common cases a low-level, implementation-specific error code may be returned.

For more details on interacting with `cy_rslt_t` see [Result Type](docs/html/group__group__result.html).

## RTOS Integration
Some HAL drivers may have slightly different behavior when run in an RTOS environment. This is typically found in operations that need to wait for a significant period of time. In an RTOS aware environment, the function will attempt to wait to use the RTOS. This allows other threads in the application to run while the current operation is waiting. In non-RTOS aware environments (e.g. bare metal environments) the functions will instead rely on busy waits for operations to complete. To inform the HAL that an RTOS environment is being used the `RTOS_AWARE` component (COMPONENTS+=RTOS_AWARE) or the `CY_RTOS_AWARE` define (DEFINES+=CY_RTOS_AWARE) must be set. When set, the HAL will use the [RTOS Abstraction] (https://github.com/infineon/abstraction-rtos) APIs to wait.

When using HAL in an RTOS environment with the `RTOS_AWARE` component enabled, setup of the HAL drivers must be done after the RTOS has been initialized to ensure that the RTOS modules such as semaphores used by the HAL drivers are initialized properly.

It is not safe to invoke more than one operation on the same HAL driver instance at a given time. If multiple threads can potentially interact with a HAL driver instance, it is the caller's responsibility to implement mutual exclusion measures (e.g. RTOS mutex, RTOS semaphore) to ensure that only one thread at a time interacts with a given instance.

## Data Cache Management
Devices such as those in the XMC72 series contain a data cache (DCACHE). When working with DCACHE, it is important to be aware of issues related to cache coherency. The HAL contains code to perform cache management, but it also requires the application to be suitably structured. Refer to [DCACHE Management](docs/html/md_asset_dcache.html) for more information on this topic.

## More information
* [API Reference Guide] (https://infineon.github.io/mtb-hal-xmc72/html/modules.html)
* [Cypress Semiconductor, an Infineon Technologies Company] (http://www.cypress.com)
* [Infineon GitHub] (https://github.com/infineon)
* [ModusToolbox™] (https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software)

---
© Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation, 2019-2024.

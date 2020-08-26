#![no_std]
#![feature(lang_items)]

#[no_mangle]
pub extern "C" fn add(a: u32, b: u32) -> u32 {
    a + b
}


mod aux {
    use core::panic::PanicInfo;

    #[panic_handler]
    fn panic(_info: &PanicInfo) -> ! {
        loop { continue }
    }

    #[lang = "eh_personality"]
    extern "C" fn eh_personality() {}
}

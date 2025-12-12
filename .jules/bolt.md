## 2024-05-23 - PIC16F Macro Definitions
**Learning:** PIC16F LCD libraries often expect specific preprocessor macros (like `RS`, `EN`, `D4`) to be defined before inclusion, rather than using namespaced ones like `LCD_RS`.
**Action:** When working with embedded C "header-only" libraries (which contain implementation), inspect them first to determine required macro definitions.

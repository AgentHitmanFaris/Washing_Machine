## 2024-05-24 - Safety Critical Interrupts vs Polling
**Vulnerability:** The emergency door sensor was implemented using polling inside a loop that also contained blocking delays (up to 3 seconds). This meant that if the door was opened during a washing cycle, the machine would not stop until the delay finished, creating a physical safety hazard.
**Learning:** In safety-critical embedded systems, polling is insufficient for emergency stops if the main loop has any blocking operations.
**Prevention:** Always use hardware interrupts (ISR) for safety-critical inputs (Emergency Stop, Door Sensors) to ensure immediate reaction regardless of what the main CPU is doing. Mark shared state variables as `volatile`.

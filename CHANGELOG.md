## v1.1
- Improved pause during attack
    - Added the ability to switch UID
    - Added the ability to emulate the current UID
    - Added the ability to save UID
- Load key file attack
    - Key file loading now does not depend on the selected protocol

## v1.0

**Supported protocols**
| iButton | RFID        |
|:-:      | :-:         |
| DS1990  | EM4100      |
| Metakom | HIDProx     |
| Cyfral  | PAC/Stanley |
|         | H10301      |

**Suported attack**
|                     | iButton | RFID |
| -                   | :-:     | :-:  |
| Default Values      | +       | +    |
| Load key file       | +       | +    |
| Load UIDs from file | +       | +    |
| BFCustomer ID       | -       | +    |
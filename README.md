# Flipper Questions

Flipper app designed to foster meaningful connections by helping you discover more about each other.

Perfect for getting in touch with new people or strengthening existing bonds.

![menu](assets/q_menu.png)

![example](assets/q_ex1.png)

![another example](assets/q_ex2.png)

## Building
1. Copy to `application_user` directory of firmware
1. Run from firmware root
```
applications_user/flipper_questions/build.sh
```

## Sending to flipper
Run from firmware root
```
applications_user/flipper_questions/send.sh
```

## Adding or modifying questions
Questions are stored as plain text files in `question_lists`. Feel free to correct, modify them or add your own lists

Questions then trasformed into C file with
```
python3 <path_to_fam>/generate_questions.py <path_to_fam>
```
or
```
applications_user/flipper_questions/build.sh
```
(both from firmware root)
# Jumail
A simple, lightweight and configurable email client made in C and using Gtk+.

## Features

- Graphical interface with context-menu based interaction
- Multiple profiles configurables via the UI
- Supports most email services such as Gmail or Outlook
- Uses the IMAP and SMTP protocols
- Supports SSL and TLS
- Multiple mailbox support
- Archive and organize emails locally
- Basic IMAP controls
    - List mails
    - Set as read/unread
    - Delete mail
    - Move mail to another mailbox
    - Send mails

## How to compile

**Required libraries:**  

- Gtk+3.0 and its dependencies
- Gmodule-2.0
- libcurl
- libxml2

The `makefile` is located in the `Debug` folder.

## Windows installer

The **windows installer** is bundled with the repository in the `installer` folder.  
If you want to build the installer yourself, the [Inno Setup](http://www.jrsoftware.org/isinfo.php) script is located in the same folder. You will have to edit it in order to make it work on your system since it uses absolute paths.  

The project was built into a VM so only the required libraries were installed. Thus, we used '\*' to select the required DLLs so be sure to edit this line in the script:

```
Source: "C:\msys64\mingw64\bin\*.dll"; DestDir: "{app}"; Flags: ignoreversion
```

## How to run

The program needs to have write access in its directory in order to work properly. It also needs to be run in the same directory as the `resources` folder which contains the Gtk Glade views.  
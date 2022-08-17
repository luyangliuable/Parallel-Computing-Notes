# FIT3143 Lab 1 
<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [FIT3143 Lab 1](#fit3143-lab-1)
    - [Finding Help](#finding-help)
        - [**Which text-based command provides information on the use of other Linux commands and utilities? List at least two.](#which-text-based-command-provides-information-on-the-use-of-other-linux-commands-and-utilities-list-at-least-two)
        - [List the command-line for finding help on the usage of ssh?](#list-the-command-line-for-finding-help-on-the-usage-of-ssh)
        - [How do you access Linux manual pages? List the full command line for accessing a](#how-do-you-access-linux-manual-pages-list-the-full-command-line-for-accessing-a)
    - [File and Directory Manipulation](#file-and-directory-manipulation)
        - [List the command-lines for creating and deleting sub-directories.](#list-the-command-lines-for-creating-and-deleting-sub-directories)
    - [Users and Access Control](#users-and-access-control)
        - [To add a new user](#to-add-a-new-user)
        - [To assignment password for new user](#to-assignment-password-for-new-user)
        - [To create a user and set login group to user](#to-create-a-user-and-set-login-group-to-user)
            - [Multiple groups](#multiple-groups)
        - [To see groups a user is part of](#to-see-groups-a-user-is-part-of)
        - [To create a user with a unique ID](#to-create-a-user-with-a-unique-id)
        - [Creating a User with an Expiry Date](#creating-a-user-with-an-expiry-date)
        - [List all users in the system](#list-all-users-in-the-system)
        - [Owner read only file](#owner-read-only-file)
        - [Owner read only home directory](#owner-read-only-home-directory)
        - [Owner write permission enable](#owner-write-permission-enable)
        - [Enable owner execute and for group to write and execute](#enable-owner-execute-and-for-group-to-write-and-execute)
    - [What does chmod 4775 filename mean?](#what-does-chmod-4775-filename-mean)
    - [How do you set the executable permission on a file? List the command-line.](#how-do-you-set-the-executable-permission-on-a-file-list-the-command-line)
        - [Enable Execute Permission on a file](#enable-execute-permission-on-a-file)
    - [List the command-line for inspecting the permissions assigned to a file or directory.](#list-the-command-line-for-inspecting-the-permissions-assigned-to-a-file-or-directory)
        - [Octal notation](#octal-notation)
        - [Linux Shell](#linux-shell)
    - [What is your default Linux shell? Read the manual pages on your shell and then](#what-is-your-default-linux-shell-read-the-manual-pages-on-your-shell-and-then)
    - [How do you get the last command-line re-displayed?](#how-do-you-get-the-last-command-line-re-displayed)
    - [Which key-stroke invokes filename completion?](#which-key-stroke-invokes-filename-completion)
    - [Which file in your home directory contains the PATH variable, what does it do, and how do you inspect its value?](#which-file-in-your-home-directory-contains-the-path-variable-what-does-it-do-and-how-do-you-inspect-its-value)
        - [To add a new path value](#to-add-a-new-path-value)
        - [Inspect PATH values](#inspect-path-values)
    - [What does the shell function alias do? Add an alias to your shell configuration file and test that it works. List the command-lines.](#what-does-the-shell-function-alias-do-add-an-alias-to-your-shell-configuration-file-and-test-that-it-works-list-the-command-lines)
    - [How does which command work? List a command-line demonstrating its function.](#how-does-which-command-work-list-a-command-line-demonstrating-its-function)
    - [How do you execute a program file in the shell? List the command-line.](#how-do-you-execute-a-program-file-in-the-shell-list-the-command-line)
    - [Why it’s sometime necessary to prefix ./ to run an executable file?](#why-its-sometime-necessary-to-prefix--to-run-an-executable-file)
    - [How are the contents of a text file displayed? List the command-line](#how-are-the-contents-of-a-text-file-displayed-list-the-command-line)
        - [Without line number](#without-line-number)
        - [With line number](#with-line-number)
        - [Count line number](#count-line-number)
    - [List the command-line for search all files with an extension .html on the system.](#list-the-command-line-for-search-all-files-with-an-extension-html-on-the-system)
    - [Create a simple shell script for listing the contents of your directory and make it executable. List the script and the command-line for changing it into an executable file.](#create-a-simple-shell-script-for-listing-the-contents-of-your-directory-and-make-it-executable-list-the-script-and-the-command-line-for-changing-it-into-an-executable-file)
    - [Networking](#networking)
        - [Using ifconfig list the following networking parameters for eth0](#using-ifconfig-list-the-following-networking-parameters-for-eth0)
            - [IP Address](#ip-address)
            - [Hardware Address](#hardware-address)
            - [NetMask](#netmask)
        - [What is the purpose of /etc/hosts file?](#what-is-the-purpose-of-etchosts-file)
        - [What is the purpose of /etc/resolv.conf file?](#what-is-the-purpose-of-etcresolvconf-file)
        - [Using NetStat display the routing table in the numeric format. List the command line and default route](#using-netstat-display-the-routing-table-in-the-numeric-format-list-the-command-line-and-default-route)

<!-- markdown-toc end -->

## Finding Help

### **Which text-based command provides information on the use of other Linux commands and utilities? List at least two. 
(**)

```shell
man
```

```shell
manual
```

### List the command-line for finding help on the usage of ssh?

```shell
man ssh
```

### How do you access Linux manual pages? List the full command line for accessing a
particular section. 

e.g. Access section 2

```shell
man 2 man
```

```shell
man man.2
```

d. Where would you generally look for help on the Web? List 3 major sites. 

1. GeeksOfGeeks
2. StackOverflow
3. Medium
4. Edstem
5. w3schools

## File and Directory Manipulation 

### List the command-lines for creating and deleting sub-directories.

* Create a sub directory

```shell
mkdir dirname
```

* Delete a sub-directory

The -r is important for recursively removing all subdirectories.

```shell
rm -rf dirname
```

b. List the command-line for creating a zero-length file. 

```shell
touch filename
```

Or if using vim or nano to start editing new file.

```shell
nano filename
```

```shell
vim filename
```

## Users and Access Control 

a. Create a user account for yourself and assign it to a unique group. List the command line. 

### To add a new user
```shell
useradd [OPTIONS] USERNAME
```


### To add a new group
```shell
groupadd [OPTIONS] GROUPNAME
```

### To assignment password for new user

```shell
sudo passwd username
```

### To create a user and set login group to user

```shell
sudo useradd -g users username
```


#### Multiple groups
```shell
sudo useradd -g users -G wheel,developers username
```

### To see groups a user is part of
```shell
groups username
```

### To create a user with a unique ID

```shell
sudo useradd -u 1500 username
id -u username
```

>> 1500

### Creating a User with an Expiry Date
```shell
sudo useradd -e 2019-01-22 username
sudo chage -l username # Vertify expiry date
```

### List all users in the system
```shell
getent passwd | grep user
```

Set the permissions for your home directory such that no one besides yourself can
read your home directory’s contents.

### Owner read only file
```shell
chmod u-w filename.txt
```

### Owner read only home directory
```shell
chmod u-w ~/
```

### Owner write permission enable
```shell
chmod u+w test1.txt
```

### Enable owner execute and for group to write and execute
```shell
chmod u+x,g+wx test1.txt
```

## What does chmod 4775 filename mean?
* The owner can read, write and execute. 

* Group can read, write and execute. 

* Others can read, can't write and can execute.

## How do you set the executable permission on a file? List the command-line. 

### Enable Execute Permission on a file
```shell
chmod +x test1.txt
```

## List the command-line for inspecting the permissions assigned to a file or directory. 

TODO These commands should work for both files and directories.

```shell
ls -l filename 
```

or

```shell
stat filename 
```

### Octal notation
Read or r is represented by 4,
Write or w is represented by 2
Execute x is represented by 1.

d (directory) <br />
c (character device)  <br />
l (symlink) <br />
p (named pipe) <br />
s (socket) <br />
b (block device) <br />
D (door) <br />
- (regular file) <br />


e.g. -rw-r--r--@
* The first - char means it is a regular file.
* rw- represents permission for the owner (read and write).
* r-- represents permission for the group (read only).
* r-- represents permission for the others (read only).

### Linux Shell 

## What is your default Linux shell? Read the manual pages on your shell and then
answer the following questions. 

/bin/bash

## How do you get the last command-line re-displayed? 
* Up arrow

## Which key-stroke invokes filename completion? 
* Tab key


## Which file in your home directory contains the PATH variable, what does it do, and how do you inspect its value?

```
. ~/.bashrc
```

### To add a new path value

Inside ~/.bashrc

```
export PATH=~/Library/Android/sdk/tools:$PATH
. ~/.bashrc # or source ~/.bashrc for some linux os
```

### Inspect PATH values

```
echo $PATH
```

## What does the shell function alias do? Add an alias to your shell configuration file and test that it works. List the command-lines.

Aliss acts as a nickname or shortcut to commands. Can be added by adding

```
alias stopmongodb="brew services stop mongodb/brew/mongodb-community"
```

To ```~/.bashrc``` or running it in the terminal.

## How does which command work? List a command-line demonstrating its function. 
Which command find the source program file of the command in the user's file path.

```shell
which node
```

>> /opt/homebrew/bin/node

## How do you execute a program file in the shell? List the command-line. 

```shell
./program_to_execute
```

e.g. node

```shell
/opt/homebrew/bin/node
```

## Why it’s sometime necessary to prefix ./ to run an executable file?
By Default when you type anything in unix terminal it checks the list of
directories specified by PATH variable. If ./ is included POSIX will just run
the filename directly.


## How are the contents of a text file displayed? List the command-line

### Without line number
```shell
cat filename
```

### With line number
```shell
nl filename
```

### Count line number
```shell
cat filename | wc -l
```

## List the command-line for search all files with an extension .html on the system. 

```shell
find  / -iname "*.html"
```

## Create a simple shell script for listing the contents of your directory and make it executable. List the script and the command-line for changing it into an executable file. 

```shell
touch test.sh
chmod u+x test.sh
./test.sh
```


## Networking


### Using ifconfig list the following networking parameters for eth0
```
lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 603  bytes 152754 (152.7 KB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 603  bytes 152754 (152.7 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

wlp59s0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 49.127.154.218  netmask 255.255.224.0  broadcast 49.127.159.255
        inet6 fe80::cd19:b6c1:3beb:30b  prefixlen 64  scopeid 0x20<link>
        ether a4:34:d9:28:59:0b  txqueuelen 1000  (Ethernet)
        RX packets 103004  bytes 146184742 (146.1 MB)
        RX errors 0  dropped 2  overruns 0  frame 0
        TX packets 33272  bytes 4136762 (4.1 MB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

#### IP Address 
* 49.127.154.218


#### Hardware Address
* fe80::cd19:b6c1:3beb:30b

#### NetMask
* 255.255.224.0

### What is the purpose of /etc/hosts file?
    It basically maps the hostnames to the ip address. It is used to map the a connection domain names and ip address when going accessing the domain name servers. It helps with translating human-friendly hostnames into numeric protocols.

### What is the purpose of /etc/resolv.conf file?
The file's purpose to list all the configured search domains.


### Using NetStat display the routing table in the numeric format. List the command line and default route

```shell
netstat -n
```

* Show Kernel routing function
```shell
netstat -n -r
```

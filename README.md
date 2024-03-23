# What is vping?
VPING is an utility for Linux. It works like `ping` command.

It's very hard to find source code of `ping` command but VPING is very similar and finding its source code is easier.

# How to run the program?
1. Download the necessary dependencies `sudo apt install git && sudo apt install gcc && sudo apt install make`
2. Download this repository `git clone https://github.com/valixxx1/vping && cd vping`
3. Compile the program `make`
4. Run it `sudo ./vping google.com`

# What does this program do?
```
user@mycomputer:~/vping$ sudo ./vping google.com
PING sending to 74.125.131.113
Data was sent... ttl = 56, seq = 1, time=65ms
Data was sent... ttl = 56, seq = 2, time=21ms
Data was sent... ttl = 56, seq = 3, time=21ms
Data was sent... ttl = 56, seq = 4, time=20ms
Data was sent... ttl = 56, seq = 5, time=20ms
Data was sent... ttl = 56, seq = 6, time=20ms
Data was sent... ttl = 56, seq = 7, time=20ms
Data was sent... ttl = 56, seq = 8, time=22ms
```

This is an output of this program. You can find Time To Life of the packets and time for their sending.

Also you can insert an IP instead of domain. For example, you can run `sudo ./vping 127.0.0.1`.

Note that you need be a superuser (root) to use this program.

# How does it works?
At first, program creates raw socket with ICMP protocol. Because of this, you need be a superuser to run the program.

Why ICMP? Because it doesn't need a port for sending, only IP. TCP and UDP packets are useless for this task.

VPING sends ICMP echo packets once per second. Then it reads echo reply from the server and prints Time To Life (ttl) and time for sending.

You can analyze the code from `main.c` file and understand how does it works.

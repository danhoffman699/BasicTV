The following is not entirely accurate for the current version, but are instead v1.0 goals. However, there is some proof of concept somewhere for everything listed, and is just a matter of implementing it.
# BasicTV
A simple, decentralized Internet television system

##What is it?
BasicTV is "a simple, decentralized Internet television system". You can flip through channels like you would a standard TV. There is no central entity. It also has:
* Programmable remote codes: you can use any free code to control BasicTV
* Remote control expandability through programmable modifier keys
* Dynamic quality, meaning that dial-up will work for streaming and 8K at 144Hz is supported (current hard limit is far beyond the technology for the next 10 years)
* Support for 3D TV (red and blue) and Oculus Rift (not a v1.0 goal, don't even own one)
* No defined upper limit for channels
* Decentralized streaming, meaning the BasicTV network does the heavy lifting (you just need enough bandwidth for one or two versions of the stream)
* No upper limit for DVR space
* DRM free DVR service
* Easy streaming, meaning you can have a live stream of anything running
* Cryptographically verified Bitcoin addresses for donation to content and BasicTV nodes
* Bitcoin "sponsorships": give fractions of a penny for every minute of watched content
* Profitability for running a node: people donating money can shave off some money automatically for whoever gave them the stream
* Networkable over the Tor network for anonymous streaming, receiving, and forwarding
* And many, many more features

##How can I get it?
There are two ways you can get this: a cable box and as a desktop program. Both of these have full node settings enabled by default, and both can be disabled if you are on a metered connection. They are literally the same piece of software with maybe a setting change for better menus.
###Cable Box
Cable box versions run the server more often, which means more information is relayed. The biggest feature is the comfort of your couch. The Raspberry Pi Zero is the model for a minimum cable box hardware setup, and will cost around $15 if you include networking and an IR receiver. A phone app is in the works to keep people from having to use IR (scan a QR code and make a TCP connection locally). It is worth stating that, for these low-end systems, CPU power is the limiting factor because of the on the fly decryption.
###Desktop Program
The desktop version does everything the cable box does in software. However, it doesn't relay nearly as much traffic, and interruptions can mean not having DVR'd whatever. Desktops tend to be more powerful than $5 ARM boards, so if you care about speed and quality of the recording and have the means to display it, you might want to opt for this.
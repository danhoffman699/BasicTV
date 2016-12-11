#ifndef ID_EXPORT_H
#define ID_EXPORT_H
/*
  When data becomes old and unpopular to host, it enters a queue to 
  be exported to another medium somehow. Optical media is cheap, but it
  has the problem of needing somebody to operate it. I also want to make
  it exportable to just another directory mounted in some way (NAS, NFS, 
  FTP). Of course, most users don't care to have to backup seemingly useless
  data to disks, so this will be a more dedicated feature.

  When requesting an ID, it should respond to each ID (or a range of IDs) a
  number indicating the storage status (available, archived, unavailable). 

  Ideally, a node will have a list of IDs, and they will try and archive the 
  oldest information first. Any item that is archived (and otherwise
  unobtainable), will repeat the request every X minutes until it becomes 
  available to one of the clients. 

  If this were a networked connection, then it would just fetch the data from
  the server and try and relay it. If it cannot do that for whatever reason 
  (network is down, server is unavailable, crash, etc.), then report it as
  unavailable. 

  If this were stored on optical media, then it would report to the user somehow
  (RSS feed, email, etc.) as they configured it (if X or more requests are 
  received in Y minutes, then notify). Ideally, it should be as easy as 
  keeping an index locally of all of the IDs and a OMID (optical media 
  identifier) and reporting to the user to insert X CD/DVD/BDR into the 
  drive. The optical media should stay to facilitate any requests with
  similar IDs (disks are optimized so they have information from around the 
  same time period, so sequential linked list entries should be on the 
  same disk).
 
  Information on disks isn't overlapped EXCEPT for the RSA public key
  information, because chunks of the data on the disk are useless without
  them (assuming either the node with the CD needs it, or the public key
  has left the network entirely). Only the public keys that are used with
  the data on the media are stored with it (would be unreasonably large, also
  would double as a possible DoS vector).

  The information on the optical media will be compressed with XZ using
  the most serious of flags (level 9 compression at extreme mode). Also,
  there will be no permissions preserved. I was considering ditching the
  ISO-9660 file system in favor of a custom system where the data is
  written raw with a few exceptions, but that's dumb.
  
  File lengths shouldn't matter, but there are places where inferior OSes
  and file systems are being used (this only applies to the FS where the
  decompression is taking places, so ISO-9660 restrictions don't apply), 
  so the files will take the following format:
  
  The SHA-256 checksum of the ID (random part) will be computed, and the first
  32 bytes will be converted into hexadecimal, this section is [SECTION A]

  The SHA-256 checksum of the ID (fingerprint) will be computed, and the first
  32 bytes will be converted into hexadecimal, this section is [SECTION B]

  The filename format shall be '[SECTION A]_[SECTION B].[DATATYPE]'

  Datatype is always lowercase with underscores and not spaces, so there should
  not be any conflict with that.

  Hashes are used to get around FS restrictions, as storing the entire ID
  in plaintext works for 256-bit (177.44) and below, but the length of the ID
  is still being questioned, so it would make sense to make this adaptable.

  The largeset optical media that a drive can handle will be detected, and
  that amount of space (plus some extra room) will be reserved for uncompressing
  the information.
 */

/*
  Since these are not used often, I wouldn't worry about sustaining
  connections (except NAS/NFS, and that's just because that's abstracted
  out).
 */

/*
  CD, DVD, and Blu-Ray
 */

struct id_export_optical_drive_t{
private:
	std::string path;
public:
	data_id_t id;
	id_export_optical_drive_t(std::string path_);
	~id_export_optical_drive_t();
};

/*
  FTP server not mounted to a directory
 */

struct id_export_ftp_t{
private:
	std::string username;
	std::string password;
public:
	data_id_t id;
	id_export_ftp_t(std::string username_,
			std::string password_);
	~id_export_ftp_t();
};

/*
  SCP/SSH
*/

struct id_export_ssh_t{
private:
	std::string username;
	std::string password;
public:
	data_id_t id;
	id_export_ssh_t(std::string username_,
			std::string password_);
	~id_export_ssh_t();
};

/*
  NAS, NFS or any other mounted medium (just another folder)
 */

struct id_export_directory_t{
private:
	std::string path;
public:
	data_id_t id;
	id_export_directory_t(std::string path);
	~id_export_directory_t();
};

namespace id_export_api{

};
#endif

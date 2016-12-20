#ifndef NET_PROTO_DEV_CTRL_H
#define NET_PROTO_DEV_CTRL_H
extern std::vector<uint8_t> net_proto_apply_dev_ctrl(std::vector<uint8_t> data);
extern std::vector<uint8_t> net_proto_unapply_dev_ctrl(std::vector<uint8_t> data);

#endif

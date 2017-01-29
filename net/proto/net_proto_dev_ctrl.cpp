#include "net_proto_dev_ctrl.h"
#include "net_proto.h"

std::vector<uint8_t> net_proto_apply_dev_ctrl(std::vector<uint8_t> data){
	for(uint64_t i = 0;i < data.size();i++){
		if(data[i] == NET_PROTO_DEV_CTRL_1){
			data.insert(data.begin()+i, NET_PROTO_DEV_CTRL_1);
			// only need one extra
			while(data[i] == NET_PROTO_DEV_CTRL_1 && i < data.size()){
				i++;
			}
		}
	}
	return data;
}

std::vector<uint8_t> net_proto_unapply_dev_ctrl(std::vector<uint8_t> data){
	for(uint64_t i = 0;i < data.size();i++){
		if(data[i] == NET_PROTO_DEV_CTRL_1){
			data.erase(data.begin()+i);
			// pos is inherently move over properly
			while(data[i] == NET_PROTO_DEV_CTRL_1 && i < data.size()){
				i++;
			}
		}
	}
	return data;
}

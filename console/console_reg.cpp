#include "console.h"

DEC_CMD(reg_set_const){
	const uint8_t entry = std::stoi(cmd_vector.at(0));
	registers.at(entry) = cmd_vector.at(1);
}

DEC_CMD(reg_set_table){
	const uint8_t entry = std::stoi(cmd_vector.at(0));
	const uint64_t x = std::stoi(cmd_vector.at(1));
	const uint64_t y = std::stoi(cmd_vector.at(2));
	registers.at(entry) = output_table.at(x).at(y);
}

DEC_CMD(reg_copy){
	const uint8_t first_entry = std::stoi(cmd_vector.at(0));
	const uint8_t second_entry = std::stoi(cmd_vector.at(1));
	registers.at(first_entry) = registers.at(second_entry);
}

DEC_CMD(reg_clear){
	const uint8_t entry = std::stoi(cmd_vector.at(0));
	registers.at(entry) = "";
}

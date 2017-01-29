#include "console.h"

static std::string gen_table(std::vector<std::vector<std::string> > entries){
	std::string retval;
	for(uint64_t x = 0;x < entries.size();x++){
		std::string row;
		for(uint64_t y = 0;y < entries[x].size();y++){
			row += "|" + entries[x][y];
		}
		row += "|";
		retval += row;
	}
	P_V_S(retval, P_DEBUG);
	return retval;
}

DEC_CMD(print_output_table){
	print_socket(
		gen_table(
			output_table)+"\n");
}

DEC_CMD(print_reg){
	print_socket(
		gen_table(
		{std::vector<std::string>(
				registers.begin(),
				registers.end())}));
}

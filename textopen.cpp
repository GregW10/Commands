#include <iostream>
#include <string>

int main(int argc, char **argv) {
	if (argc != 2)
		return 1;
	std::string cmd("open ");
	cmd.append(*(argv + 1));
	cmd.append(" -a \"TextEdit\"");
	system(cmd.c_str());
	return 0;
}

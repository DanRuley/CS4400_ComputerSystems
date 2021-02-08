#include <iostream>
#include <string>

int main(){
  std::string input;
  bool seenparen;

  while( std::getline(std::cin, input) ){
    seenparen = false;
    for(int i = 0; i < input.length(); i++){
      if(input[i] == '(')
        seenparen = true;
      else if(input[i] == ')')
        seenparen = false;
      else if(seenparen && input[i] >= '0' && input[i] <= '9')
        input[i] = '*';
    }
    std::cout << input << std::endl;
  }
  return 0;
}

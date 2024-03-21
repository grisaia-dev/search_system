#include "parser.hpp"

INIP::Parser::Parser(const std::filesystem::path filePath) {
    file_ini.open(filePath, std::ios::binary | std::ios::in);
    std::string forEx = "File " + filePath.string() + " not found!";

    try {
        if (!file_ini.is_open())
            throw Exception_runtime(forEx);
    } catch (const std::runtime_error& ex) {
        std::cout << HIT << ex.what() << std::endl;
        std::cout << HIT << "Creating config file.." << std::endl;
        create_file_config();
        file_ini.open(filePath, std::ios::binary | std::ios::in);
    }

    file_ini.seekg(0, std::ios::end); // Перемещаем указатель что бы узнать размер данных
	size_t fileSize = file_ini.tellg(); // сохраняем размер данных
	file_ini.seekg(0, std::ios::beg); // переносим в первоначальное положение
	
	std::string fileData(fileSize, ' ');
	file_ini.read(&fileData[0], fileSize); // Переносим данные из файла в строку

    ProcessRead(fileData);
}

INIP::Parser::~Parser() {
    file_ini.close();
}

void INIP::Parser::ProcessRead(const std::string_view& str) {
    std::cout << HIT << "Reading a configuration file.." << std::endl;
	try {
		Reset();
		for (char c : str) {
			// Cath control keys
			if (std::iscntrl(c)) {
				switch (c) {
					case '\t':
						[[fallthrough]];
					case '\n':
						break;
					default:
						continue;
				}
			} // end if (std::iscntrl(c))

			switch (m_state) {
				case State::ReadyForData:
					if (c == ';') {
						m_state = State::Comment;
                    }    
					else if (c == '[') {
						m_currentSection.clear();
						m_state = State::Section;
					} else if (c == ' ')
						continue;
					else if (c == '\t')
						continue;
					else if (c == '\n') {
                        m_line += 1;
						continue;
					} else {
						m_currentKey.clear();
						m_currentKey += c;
						m_state = State::Key;
					}
					break;
				case State::Comment:
					if (c == '\n') {
						m_state = State::ReadyForData;
						m_line += 1;
					}
					break;
				case State::Section:
					if (c == ']') {
						m_state = State::ReadyForData;
					} else if (c == '\t')
						throw Exception_runtime("Tabs are not allowed in section name! At line: " + std::to_string(m_line));
					else if (c == '\n')
						throw Exception_runtime("Newlines are not allowed in section name! At line: " + std::to_string(m_line));
					else
						m_currentSection += c;
					break;
				case State::Key:
					if (c == ' ')
						m_state = State::KeyDone;
					else if (c == '=')
						m_state = State::Equal;
					else if (c == '\t') {
						//throw Exception("Tabs are not allowed in the key! At line: " + std::to_string(line)); // если хотим что бы нельзя было делать табуляцию после ключа
						continue;
					} else if (c == '\n') {
						throw Exception_runtime("Newlines are not allowed in the key! At line: " + std::to_string(m_line));
					} else
						m_currentKey += c;
					break;
				case State::KeyDone:
					if (c == ' ')
						continue;
					else if (c == '\t')
						continue;
					else if (c == '\n')
						throw Exception_runtime("not found '='! At line: " + std::to_string(m_line));
					else if (c == '=')
						m_state = State::Equal;
					else
						throw Exception_runtime("Keys are not allowed to have spaces in them! At line: " + std::to_string(m_line));
					break;
				case State::Equal:
					if (c == ' ')
						continue;
					else if (c == '\t')
						continue;
					else if (c == '\n')
						throw Exception_runtime("Value can't be empty! At line: " + std::to_string(m_line));
					else {
						m_currentValue.clear();
						m_currentValue += c;
						m_state = State::Variable;
					}
					break;
				case State::Variable:
					if (c == '\n') {
						Pair(m_currentSection, m_currentKey, m_currentValue);
						m_line += 1;
						m_state = State::ReadyForData;
					} else if (c == ';') {
						Pair(m_currentSection, m_currentKey, m_currentValue);
						m_state = State::Comment;
					} else {
						m_currentValue += c;
					}
					break;
			} // end switch (m_state)
		} // end for
    std::cout << GOOD << "Reading complete!" << std::endl;
	} catch (const std::runtime_error& ex) {
		std::cout << ERROR << ex.what() << std::endl;
		Parser::~Parser();
        exit(1);
	}
} // end function

void INIP::Parser::Reset() {
	m_currentSection = "";
	m_currentKey = "";
	m_currentValue = "";
	m_state = State::ReadyForData;
}

void INIP::Parser::Pair(const std::string& section, const std::string& key, const std::string& value) {
	std::string temp = section + "." + key;
	if (m_sKeyValue.count(temp))
		m_sKeyValue[temp] = value;
	else if (!m_sKeyValue.count(temp))
		m_sKeyValue.emplace(temp, value);
}

void INIP::Parser::create_file_config() {
    std::ofstream file("config.ini");
    std::cout << GOOD << "File created!" << std::endl;
    std::cout << HIT << "Filling out the file.." << std::endl;
    std::string options = "; База данных\n"
                            "[DB]\n"
                            "\thost = localhost\n"
                            "\tport = 5432\n"
                            "\tdbname = postgres\n"
                            "\tuser = postgres\n"
                            "\tpassword = postgres\n\n"
							"; Сайт который нужно парсить\n"
							"[SITE]\n"
							"\taddress = https://<Укажите сайт>\n"
							"\trec = 1 ; Укажите глубину парсинга\n"
							"\tport = <what> ; Укажите порт для запуска программы поисковика\n";
    file << options;
    file.close();
}
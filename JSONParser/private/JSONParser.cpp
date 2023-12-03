#include "JSONParser.h"

#include "Files.h"
#include "parser.h"
#include "JSONBuilder.h"

#include "codeSource.h"

namespace
{
	struct ParserDataContainer
	{
		scripting::Grammar* m_grammar = nullptr;
		scripting::Parser* m_parser = nullptr;
		scripting::ParserTable m_parserTable;

		void Init(const std::string& grammarSrc)
		{
			m_grammar = new scripting::Grammar(grammarSrc);
			m_grammar->GenerateParserStates();
			m_grammar->GenerateParserTable(m_parserTable);

			bool valid = m_parserTable.Validate();

			m_parser = new scripting::Parser(*m_grammar, m_parserTable);
		}

		~ParserDataContainer()
		{
			if (m_grammar)
			{
				delete m_grammar;
			}

			if (m_parser)
			{
				delete m_parser;
			}
		}
	};

	ParserDataContainer m_grammarContainer;
}

void json_parser::Boot()
{
	std::string grammarSrc;
	files::ReadTextFile("json_grammar.txt", grammarSrc);
	
	m_grammarContainer.Init(grammarSrc);
	
#if false
	std::string testJson;
	files::ReadTextFile("test_json.json", testJson);

	scripting::CodeSource cs;
	cs.m_code = testJson;
	cs.Tokenize();
	cs.TokenizeForJSONReader();

	scripting::ISymbol* symbol = m_grammarContainer.m_parser->Parse(cs);
	JSONValue val;
	ValueBuilder vb(symbol);
	vb.Build(val);

	std::string str = val.ToString(true);

	JSONValue obj(ValueType::Object);
	auto& map = obj.GetAsObj();
	map["dw"] = JSONValue("efewr");
	str = obj.ToString(true);
	bool t = true;
#endif

}

scripting::ISymbol* json_parser::Parse(scripting::CodeSource& codeSource)
{
	return m_grammarContainer.m_parser->Parse(codeSource);
}

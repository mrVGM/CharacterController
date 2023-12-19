#include "XMLReader.h"

#include "XMLReader.h"
#include "Files.h"
#include "parserTable.h"
#include "parser.h"
#include "grammar.h"
#include "codeSource.h"
#include "TreeBuilder.h"

#include <map>
#include <queue>

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

	void AssignParents(const std::list<xml_reader::Node*>& nodes)
	{
		using namespace xml_reader;

		std::set<Node*> processed;

		std::queue<Node*> toProcess;

		for (auto it = nodes.begin(); it != nodes.end(); ++it)
		{
			toProcess.push(*it);
		}

		while (!toProcess.empty())
		{
			Node* cur = toProcess.front();
			toProcess.pop();

			if (processed.contains(cur))
			{
				continue;
			}

			for (auto it = cur->m_children.begin(); it != cur->m_children.end(); ++it)
			{
				Node* curChild = *it;
				curChild->m_parent = cur;
				toProcess.push(curChild);
			}
			processed.insert(cur);
		}
	}
}

void xml_reader::ReadXML(const std::string& src, XMLTree& xmlTree)
{
	scripting::CodeSource cs;
	cs.m_code = src;

	cs.Tokenize();
	cs.TokenizeForColladaReader();

	scripting::ISymbol* symbol = m_grammarContainer.m_parser->Parse(cs);

	if (!symbol)
	{
		throw "Bad XML!";
	}
	
	xml_reader::TreeBuilder builder(xmlTree.m_allNodes);
	bool res = builder.BuildTree(symbol);
	if (!res)
	{
		throw "Bad XML!";
	}

	AssignParents(xmlTree.m_allNodes);
	xmlTree.m_rootNodes = builder.m_rootNodes;
}

void xml_reader::Boot()
{
	std::string grammarSrc;
	files::ReadTextFile("collada_grammar.txt", grammarSrc);
	m_grammarContainer.Init(grammarSrc);
}

xml_reader::XMLTree::~XMLTree()
{
	for (auto it = m_allNodes.begin(); it != m_allNodes.end(); ++it)
	{
		delete *it;
	}

	m_allNodes.clear();
	m_rootNodes.clear();
}

void xml_reader::XMLTree::FindNodes(const std::function<bool(const Node*)>& predicate, std::list<const Node*>& outNodes) const
{
	for (auto it = m_allNodes.begin(); it != m_allNodes.end(); ++it)
	{
		if (predicate(*it))
		{
			outNodes.push_back(*it);
		}
	}
}

const xml_reader::Node* xml_reader::XMLTree::FindNode(const std::function<bool(const Node*)>& predicate) const
{
	for (auto it = m_allNodes.begin(); it != m_allNodes.end(); ++it)
	{
		if (predicate(*it))
		{
			return *it;
		}
	}

	return nullptr;
}

void xml_reader::XMLTree::FindChildNodes(
	const Node* rootNode,
	const std::function<bool(const Node*)>& predicate,
	bool directChild,
	std::list<const Node*>& outNodes) const
{
	if (directChild)
	{
		for (auto it = rootNode->m_children.begin(); it != rootNode->m_children.end(); ++it)
		{
			if (predicate(*it))
			{
				outNodes.push_back(*it);
			}
		}

		return;
	}

	std::queue<const Node*> nodes;
	nodes.push(rootNode);

	while (!nodes.empty())
	{
		const Node* cur = nodes.front();
		nodes.pop();

		if (predicate(cur))
		{
			outNodes.push_back(cur);
		}

		for (auto it = cur->m_children.begin(); it != cur->m_children.end(); ++it)
		{
			nodes.push(*it);
		}
	}
}

const xml_reader::Node* xml_reader::XMLTree::FindChildNode(
	const Node* rootNode,
	const std::function<bool(const Node*)>& predicate,
	bool directChild) const
{
	if (directChild)
	{
		for (auto it = rootNode->m_children.begin(); it != rootNode->m_children.end(); ++it)
		{
			if (predicate(*it))
			{
				return *it;
			}
		}

		return nullptr;
	}

	std::queue<const Node*> nodes;
	nodes.push(rootNode);

	while (!nodes.empty())
	{
		const Node* cur = nodes.front();
		nodes.pop();

		if (predicate(cur))
		{
			return cur;
		}

		for (auto it = cur->m_children.begin(); it != cur->m_children.end(); ++it)
		{
			nodes.push(*it);
		}
	}

	return nullptr;
}

#include "Mesh.h"

#include "PrimitiveTypes.h"

#include "Jobs.h"

#include "MemoryFile.h"
#include "Files.h"
#include "XMLReader.h"
#include "Hash.h"

#include "MathUtils.h"
#include "Geometry.h"

#include "JSONValue.h"

#include <map>
#include <string>
#include <vector>
#include <list>
#include <sstream>

namespace
{
	BasicObjectContainer<geo::MeshTypeDef> m_meshTypeDef;

	struct MeshReader
	{
		const xml_reader::XMLTree& m_tree;
		const xml_reader::Node* m_meshNode = nullptr;

		std::vector<math::Vector3> m_positions;
		std::vector<math::Vector3> m_normals;
		std::vector<math::Vector2> m_uvs;

		std::vector<geo::MeshVertex> m_verts;
		std::map<std::string, int> m_vertexMap;

		MeshReader(const xml_reader::XMLTree& tree) :
			m_tree(tree)
		{
		}

		const xml_reader::Node* GetMeshNode()
		{
			using namespace xml_reader;

			if (m_meshNode)
			{
				return m_meshNode;
			}

			const Node* libGeometries = m_tree.FindNode([](const Node* node) {
				return node->m_tagName == "library_geometries";
			});

			const Node* geometry = m_tree.FindChildNode(libGeometries, [](const Node* node) {
				return node->m_tagName == "geometry";
			}, true);

			const Node* mesh = m_tree.FindChildNode(geometry, [](const Node* node) {
				return node->m_tagName == "mesh";
			}, true);

			m_meshNode = mesh;
			return m_meshNode;
		}

		void GetTrianglesNodes(std::list<const xml_reader::Node*>& outTrianglesNodes)
		{
			using namespace xml_reader;

			m_tree.FindChildNodes(GetMeshNode(), [](const Node* node) {
				return node->m_tagName == "triangles";
			}, true, outTrianglesNodes);
		}

		void ReadVector3s(const xml_reader::Node* source, std::vector<math::Vector3>& vects)
		{
			if (!vects.empty())
			{
				return;
			}

			using namespace xml_reader;

			const Node* accessor = m_tree.FindChildNode(
				source,
				[](const Node* node) {
					return node->m_tagName == "accessor";
				},
				false);

			std::string accessorSourceId = accessor->m_tagProps.find("source")->second.c_str() + 1;

			const Node* floatArray = m_tree.FindChildNode(
				source,
				[&accessorSourceId](const Node* node) {
					if (node->m_tagName != "float_array")
					{
						return false;
					}

					return node->m_tagProps.find("id")->second == accessorSourceId;
				},
				true);

			int xIndex = -1;
			int yIndex = -1;
			int zIndex = -1;

			int index = 0;
			for (auto it = accessor->m_children.begin(); it != accessor->m_children.end(); ++it)
			{
				const Node* cur = *it;
				if (cur->m_tagName != "param")
				{
					continue;
				}
				int curIndex = index++;

				if (cur->m_tagProps.find("name")->second == "X")
				{
					xIndex = curIndex;
					continue;
				}

				if (cur->m_tagProps.find("name")->second == "Y")
				{
					yIndex = curIndex;
					continue;
				}

				if (cur->m_tagProps.find("name")->second == "Z")
				{
					zIndex = curIndex;
					continue;
				}
			}

			std::stringstream ss;
			ss << (accessor->m_tagProps.find("count")->second);
			int count;
			ss >> count;
			ss.clear();

			ss << (accessor->m_tagProps.find("stride")->second);
			int stride;
			ss >> stride;
			ss.clear();

			auto it = floatArray->m_data.begin();
			for (int i = 0; i < count; ++i)
			{
				math::Vector3& vect = vects.emplace_back();
				for (int j = 0; j < stride; ++j)
				{
					scripting::ISymbol* cur = *it;
					++it;
					if (j == xIndex)
					{
						vect.m_coefs[0] = cur->m_symbolData.m_number;
						continue;
					}
					if (j == yIndex)
					{
						vect.m_coefs[1] = cur->m_symbolData.m_number;
						continue;
					}
					if (j == zIndex)
					{
						vect.m_coefs[2] = cur->m_symbolData.m_number;
						continue;
					}
				}
			}
		}

		void ReadUVs(const xml_reader::Node* source, std::vector<math::Vector2>& uvs)
		{
			if (!uvs.empty())
			{
				return;
			}

			using namespace xml_reader;

			const Node* accessor = m_tree.FindChildNode(
				source,
				[](const Node* node) {
					return node->m_tagName == "accessor";
				},
				false);

			std::string accessorSourceId = accessor->m_tagProps.find("source")->second.c_str() + 1;

			const Node* floatArray = m_tree.FindChildNode(
				source,
				[&accessorSourceId](const Node* node) {
					if (node->m_tagName != "float_array")
					{
						return false;
					}

					return node->m_tagProps.find("id")->second == accessorSourceId;
				},
				true);

			int xIndex = -1;
			int yIndex = -1;

			int index = 0;
			for (auto it = accessor->m_children.begin(); it != accessor->m_children.end(); ++it)
			{
				const Node* cur = *it;
				if (cur->m_tagName != "param")
				{
					continue;
				}
				int curIndex = index++;

				if (cur->m_tagProps.find("name")->second == "S")
				{
					xIndex = curIndex;
					continue;
				}

				if (cur->m_tagProps.find("name")->second == "T")
				{
					yIndex = curIndex;
					continue;
				}
			}

			std::stringstream ss;
			ss << (accessor->m_tagProps.find("count")->second);
			int count;
			ss >> count;
			ss.clear();

			ss << (accessor->m_tagProps.find("stride")->second);
			int stride;
			ss >> stride;
			ss.clear();

			auto it = floatArray->m_data.begin();
			for (int i = 0; i < count; ++i)
			{
				math::Vector2& vect = uvs.emplace_back();
				for (int j = 0; j < stride; ++j)
				{
					scripting::ISymbol* cur = *it;
					++it;
					if (j == xIndex)
					{
						vect.m_coefs[0] = cur->m_symbolData.m_number;
						continue;
					}
					if (j == yIndex)
					{
						vect.m_coefs[1] = cur->m_symbolData.m_number;
						continue;
					}
				}
			}
		}

		void ReadTriangles(const xml_reader::Node* triangles, std::list<int>& indices)
		{
			using namespace xml_reader;
			const Node* vertexInput = m_tree.FindChildNode(
				triangles,
				[](const Node* node) {
					if (node->m_tagName != "input") {
						return false;
					}
					return node->m_tagProps.find("semantic")->second == "VERTEX";
				},
				true);

			const Node* normalInput = m_tree.FindChildNode(
				triangles,
				[](const Node* node) {
					if (node->m_tagName != "input") {
						return false;
					}
					return node->m_tagProps.find("semantic")->second == "NORMAL";
				},
				true);

			const Node* uvInput = m_tree.FindChildNode(
				triangles,
				[](const Node* node) {
					if (node->m_tagName != "input") {
						return false;
					}
					return node->m_tagProps.find("semantic")->second == "TEXCOORD";
				},
				true);

			const Node* vertexSourceNode = nullptr;
			const Node* normalSourceNode = nullptr;
			const Node* uvSourceNode = nullptr;

			{
				std::string vertsId = vertexInput->m_tagProps.find("source")->second.c_str() + 1;
				const Node* vertices = m_tree.FindChildNode(
					GetMeshNode(),
					[&vertsId](const Node* node) {
						if (node->m_tagName != "vertices")
						{
							return false;
						}

						return node->m_tagProps.find("id")->second == vertsId;
					},
					true);

				const Node* positionsInput = m_tree.FindChildNode(
					vertices,
					[](const Node* node) {
						if (node->m_tagName != "input")
						{
							return false;
						}

						return node->m_tagProps.find("semantic")->second == "POSITION";
					}, true);

				std::string vertexSourceId = positionsInput->m_tagProps.find("source")->second.c_str() + 1;

				vertexSourceNode = m_tree.FindChildNode(
					GetMeshNode(),
					[&vertexSourceId](const Node* node) {
						if (node->m_tagName != "source")
						{
							return false;
						}

						return node->m_tagProps.find("id")->second == vertexSourceId;
					},
					true);
			}

			{
				std::string normalSourceId = normalInput->m_tagProps.find("source")->second.c_str() + 1;
				normalSourceNode = m_tree.FindChildNode(
					GetMeshNode(),
					[&normalSourceId](const Node* node) {
						if (node->m_tagName != "source")
						{
							return false;
						}

						return node->m_tagProps.find("id")->second == normalSourceId;
					},
					true);

			}

			{
				std::string uvSourceId = uvInput->m_tagProps.find("source")->second.c_str() + 1;
				uvSourceNode = m_tree.FindChildNode(
					GetMeshNode(),
					[&uvSourceId](const Node* node) {
						if (node->m_tagName != "source")
						{
							return false;
						}

						return node->m_tagProps.find("id")->second == uvSourceId;
					},
					true);

			}

			ReadVector3s(vertexSourceNode, m_positions);
			ReadVector3s(normalSourceNode, m_normals);
			ReadUVs(uvSourceNode, m_uvs);

			const Node* p = m_tree.FindChildNode(
				triangles,
				[](const Node* node) {
					return node->m_tagName == "p";
				},
				true);

			int count;
			std::stringstream ss;
			ss << triangles->m_tagProps.find("count")->second;
			ss >> count;

			int stride = 0;
			{
				std::list<const Node*> inputNodes;
				m_tree.FindChildNodes(
					triangles,
					[](const Node* node) {
						return node->m_tagName == "input";
					},
					true,
					inputNodes);
				stride = inputNodes.size();
			}

			int vertexOffset = -1;
			int normalOffset = -1;
			int uvOffset = -1;

			{
				ss.clear();
				ss << vertexInput->m_tagProps.find("offset")->second;
				ss >> vertexOffset;

				ss.clear();
				ss << normalInput->m_tagProps.find("offset")->second;
				ss >> normalOffset;

				ss.clear();
				ss << uvInput->m_tagProps.find("offset")->second;
				ss >> uvOffset;
			}

			auto pIt = p->m_data.begin();
			for (int i = 0; i < count; ++i)
			{
				for (int k = 0; k < 3; ++k)
				{

					int vertIndex = -1;
					int normalIndex = -1;
					int uvIndex = -1;

					for (int j = 0; j < stride; ++j)
					{
						scripting::ISymbol* cur = *pIt;
						++pIt;

						if (j == vertexOffset)
						{
							vertIndex = static_cast<int>(cur->m_symbolData.m_number);
							continue;
						}
						if (j == normalOffset)
						{
							normalIndex = static_cast<int>(cur->m_symbolData.m_number);
							continue;
						}
						if (j == uvOffset)
						{
							uvIndex = static_cast<int>(cur->m_symbolData.m_number);
							continue;
						}
					}

					json_parser::JSONValue key(json_parser::ValueType::Object);
					{
						auto& map = key.GetAsObj();
						map["position"] = json_parser::JSONValue(static_cast<float>(vertIndex));
						map["normal"] = json_parser::JSONValue(static_cast<float>(normalIndex));
						map["uv"] = json_parser::JSONValue(static_cast<float>(uvIndex));
					}

					std::string keyStr = key.ToString(false);

					int meshVertexIndex = -1;
					auto vertIt = m_vertexMap.find(keyStr);
					if (vertIt == m_vertexMap.end())
					{
						geo::MeshVertex& vert = m_verts.emplace_back();
						vert.m_position = m_positions[vertIndex];
						vert.m_normal = m_normals[normalIndex] ;
						vert.m_uv = m_uvs[uvIndex];
						meshVertexIndex = m_verts.size() - 1;
						m_vertexMap[keyStr] = meshVertexIndex;
					}
					else
					{
						meshVertexIndex = vertIt->second;
					}

					indices.push_back(meshVertexIndex);
				}
			}
		}
	};
}

geo::MeshTypeDef::MeshTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "D963BDD2-9E53-466B-BCAF-F3FB50434050"),
	m_colladaFile("D9F8FC86-02D0-4EAF-A73E-8616AB554825", StringTypeDef::GetTypeDef()),
	m_hash("11D91937-B683-4FC8-B175-37A3670916F2", StringTypeDef::GetTypeDef())
{
	{
		m_colladaFile.m_name = "Collada File";
		m_colladaFile.m_category = "Setup";
		m_colladaFile.m_getValue = [](CompositeValue* obj) -> Value& {
			Mesh* mesh = static_cast<Mesh*>(obj);
			return mesh->m_colladaFile;
		};
		m_properties[m_colladaFile.GetId()] = &m_colladaFile;
	}

	{
		m_hash.m_name = "Hash";
		m_hash.m_category = "Internal";
		m_hash.m_getValue = [](CompositeValue* obj) -> Value& {
			Mesh* mesh = static_cast<Mesh*>(obj);
			return mesh->m_hash;
		};
		m_properties[m_hash.GetId()] = &m_hash;
	}

	m_name = "Mesh";
	m_category = "Geometry";
}

geo::MeshTypeDef::~MeshTypeDef()
{
}

void geo::MeshTypeDef::Construct(Value& container) const
{
	Mesh* mesh = new Mesh(*this);
	container.AssignObject(mesh);
}

const geo::MeshTypeDef& geo::MeshTypeDef::GetTypeDef()
{
	if (!m_meshTypeDef.m_object)
	{
		m_meshTypeDef.m_object = new MeshTypeDef();
	}

	return *m_meshTypeDef.m_object;
}

geo::Mesh::Mesh(const ReferenceTypeDef& type) :
	ObjectValue(type),
	m_loader(*this),
	m_hash(MeshTypeDef::GetTypeDef().m_hash.GetType(), this),
	m_colladaFile(MeshTypeDef::GetTypeDef().m_colladaFile.GetType(), this),
	m_buffers(ReferenceTypeDef::GetTypeDef(), this)
{
}

geo::Mesh::~Mesh()
{
	if (m_vertices)
	{
		delete[] m_vertices;
	}
	if (m_indices)
	{
		delete[] m_indices;
	}

	m_vertices = nullptr;
	m_indices = nullptr;
}

void geo::Mesh::Load(jobs::Job* done)
{
	m_loader.Load(done);
}

void geo::Mesh::LoadData(jobs::Job* done)
{
	using namespace xml_reader;

	std::string colladaFile = "Geometry\\" + m_colladaFile.Get<std::string>();

	std::string contents;
	files::ReadTextFile(colladaFile, contents);

	std::string hash = crypto::HashString(contents);

	xml_reader::XMLTree tree;
	xml_reader::ReadXML(contents, tree);

	const Node* upAxis = tree.FindNode([](const Node* node) {
		return node->m_tagName == "up_axis";
	});

	m_zUp = upAxis->m_data.front()->m_symbolData.m_string == "Z_UP";

	const Node* libGeometries = tree.FindNode([](const Node* node) {
		return node->m_tagName == "library_geometries";
	});

	const Node* geometry = tree.FindChildNode(libGeometries, [](const Node* node) {
		return node->m_tagName == "geometry";
	}, true);

	const Node* mesh = tree.FindChildNode(geometry, [](const Node* node) {
		return node->m_tagName == "mesh";
	}, true);

	std::list<const Node*> triangles;
	tree.FindChildNodes(mesh, [](const Node* node) {
		return node->m_tagName == "triangles";
	}, true, triangles);

	m_numIndices = 0;
	std::list<std::list<int>> indices;
	MeshReader mr(tree);
	for (auto it = triangles.begin(); it != triangles.end(); ++it)
	{
		std::list<int>& curIndices = indices.emplace_back();
		mr.ReadTriangles(*it, curIndices);
		MaterialRange& range = m_materials.emplace_back();
		range.m_start = m_numIndices;
		range.m_count = curIndices.size();
		m_numIndices += curIndices.size();
	}

	m_numVertices = mr.m_verts.size();
	m_vertices = new MeshVertex[m_numVertices];
	{
		int index = 0;
		for (auto it = mr.m_verts.begin(); it != mr.m_verts.end(); ++it)
		{
			MeshVertex& cur = m_vertices[index++];
			cur = *it;

			if (m_zUp)
			{
				float tmp = cur.m_position.m_coefs[1];
				cur.m_position.m_coefs[1] = cur.m_position.m_coefs[2];
				cur.m_position.m_coefs[2] = tmp;

				tmp = cur.m_normal.m_coefs[1];
				cur.m_normal.m_coefs[1] = cur.m_normal.m_coefs[2];
				cur.m_normal.m_coefs[2] = tmp;
			}
		}
	}

	m_indices = new int[m_numIndices];
	{
		int index = 0;
		for (auto it = indices.begin(); it != indices.end(); ++it)
		{
			const std::list<int>& cur = *it;
			for (auto curIt = cur.begin(); curIt != cur.end(); ++curIt)
			{
				m_indices[index++] = *curIt;
			}
		}

		if (m_zUp)
		{
			for (int i = 0; i < m_numIndices; i += 3)
			{
				int tmp = m_indices[i + 1];
				m_indices[i + 1] = m_indices[i + 2];
				m_indices[i + 2] = tmp;
			}
		}
	}

	files::MemoryFile mf;
	SerializeToMF(mf);

	std::string id = GetTypeDef().GetId();
	mf.SaveToFile(files::GetDataDir() + files::GetAssetsBinDir() + id + ".bin");

	jobs::RunSync(done);
}

void geo::Mesh::SerializeToMF(files::MemoryFile& mf)
{
	using namespace files;
	
	MemoryFileWriter writer(mf);

	{
		BinChunk vertexChunk;
		vertexChunk.m_data = reinterpret_cast<char*>(new MeshVertex[m_numVertices]);
		vertexChunk.m_size = m_numVertices * sizeof(MeshVertex);
		memcpy(vertexChunk.m_data, m_vertices, vertexChunk.m_size);

		vertexChunk.Write(writer);
	}

	{
		BinChunk indexChunk;
		indexChunk.m_data = reinterpret_cast<char*>(new int[m_numIndices]);
		indexChunk.m_size = m_numIndices * sizeof(int);
		memcpy(indexChunk.m_data, m_indices, indexChunk.m_size);

		indexChunk.Write(writer);
	}
}

void geo::Mesh::DeserializeFromMF(files::MemoryFile& mf)
{
	using namespace files;

	MemoryFileReader reader(mf);

	{
		BinChunk vertexChunk;
		vertexChunk.Read(reader);

		m_numVertices = vertexChunk.m_size / sizeof(MeshVertex);
		m_vertices = new MeshVertex[m_numVertices];

		memcpy(m_vertices, vertexChunk.m_data, vertexChunk.m_size);
	}

	{
		BinChunk indexChunk;
		indexChunk.Read(reader);

		m_numIndices = indexChunk.m_size / sizeof(int);
		m_indices = new int[m_numIndices];

		memcpy(m_indices, indexChunk.m_data, indexChunk.m_size);
	}
}
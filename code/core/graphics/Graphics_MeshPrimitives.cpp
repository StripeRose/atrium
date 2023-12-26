#include "Graphics_Mesh.hpp"

#include <vector>

namespace RoseGold::Core::Graphics
{
	namespace MakeMeshPrimitives
	{
		using namespace RoseGold::Math;

		struct GeneratedTriangle
		{
			GeneratedTriangle(Vector3 aPosA, Vector3 aPosB, Vector3 aPosC, Vector2 aUVA, Vector2 aUVB, Vector2 aUVC)
			{
				Position[0] = aPosA;
				Position[1] = aPosB;
				Position[2] = aPosC;
				UV[0] = aUVA;
				UV[1] = aUVB;
				UV[2] = aUVC;
			}
			Vector3 Position[3];
			Vector2 UV[3];
		};

		void PopulateQuad(std::vector<Vertex>& someVertices, std::vector<Triangle>& someTriangles);

		/*void AddTriangle(const Vertex aTriangle[3], bool mergeVertices, std::vector<Vertex>& someVertices, std::vector<Triangle>& someTriangles)
		{
			auto findOrAddVertex = [&someVertices, mergeVertices](const Vertex& aVertex) -> std::uint32_t {
				std::size_t vertexIndex = someVertices.size();
				if (mergeVertices)
				{
					for (std::size_t i = 0; i < someVertices.size(); ++i)
					{
						const Vertex& v = someVertices[i];
						if (v.Position == aVertex.Position && v.UV == aVertex.UV && v.Normal == aVertex.Normal)
							vertexIndex = i;
					}
				}

				if (vertexIndex == someVertices.size())
					someVertices.push_back(aVertex);

				return static_cast<std::uint32_t>(vertexIndex);
			};

			Triangle newTriangle;
			newTriangle.V1 = findOrAddVertex(aTriangle[0]);
			newTriangle.V2 = findOrAddVertex(aTriangle[1]);
			newTriangle.V3 = findOrAddVertex(aTriangle[2]);
			someTriangles.push_back(newTriangle);
		}*/

		void GenerateNormals_Flat(std::vector<Vertex>& someVertices, const std::vector<Triangle>& someTriangles)
		{
			for (const Triangle& tri : someTriangles)
			{
				Vertex& v1 = someVertices[tri.V1];
				Vertex& v2 = someVertices[tri.V2];
				Vertex& v3 = someVertices[tri.V3];

				const Vector3 A = Vector3(v1.Position.X, v1.Position.Y, v1.Position.Z);
				const Vector3 B = Vector3(v2.Position.X, v2.Position.Y, v2.Position.Z);
				const Vector3 C = Vector3(v3.Position.X, v3.Position.Y, v3.Position.Z);

				v1.Tangent = (B - A).Normalized();
				v1.Binormal = (C - A).Normalized();
				v1.Normal = Vector3::Cross(v1.Tangent, v1.Binormal);

				v3.Tangent = v2.Tangent = v1.Tangent;
				v3.Binormal = v2.Binormal = v1.Binormal;
				v3.Normal = v2.Normal = v1.Normal;
			}
		}

		void GenerateNormals_Smooth(std::vector<Vertex>& someVertices, const std::vector<Triangle>& someTriangles)
		{
			for (Vertex& vert : someVertices)
			{
				vert.Normal = Vector3::Zero();
				vert.Binormal = Vector3::Zero();
				vert.Tangent = Vector3::Zero();
			}

			for (const Triangle& tri : someTriangles)
			{
				Vertex& v1 = someVertices[tri.V1];
				Vertex& v2 = someVertices[tri.V2];
				Vertex& v3 = someVertices[tri.V3];

				const Vector3 A = Vector3(v1.Position.X, v1.Position.Y, v1.Position.Z);
				const Vector3 B = Vector3(v2.Position.X, v2.Position.Y, v2.Position.Z);
				const Vector3 C = Vector3(v3.Position.X, v3.Position.Y, v3.Position.Z);

				v1.Tangent = (B - A).Normalized();
				v1.Binormal = (C - A).Normalized();

				const Vector3 vertexNormal = Vector3::Cross(v1.Tangent, v1.Binormal);
				v1.Normal += vertexNormal;
				v2.Normal += vertexNormal;
				v3.Normal += vertexNormal;

				v3.Tangent = v2.Tangent = v1.Tangent;
				v3.Binormal = v2.Binormal = v1.Binormal;
			}

			for (Vertex& vert : someVertices)
			{
				vert.Normal.Normalize();
				vert.Binormal.Normalize();
				vert.Tangent.Normalize();
			}
		}

		/*void Subdivide(std::vector<Vertex>& someVertices, std::vector<Triangle>& someTriangles)
		{
			const std::vector<Triangle> trianglesToSubdivide = someTriangles;
			someTriangles.clear();

			for (const Triangle& triangle : trianglesToSubdivide)
			{
				const Vertex v1 = someVertices[triangle.V1];
				const Vertex v2 = someVertices[triangle.V2];
				const Vertex v3 = someVertices[triangle.V3];

				const Vertex midpoint12 = Vertex::Lerp(v1, v2, 0.5f);
				const Vertex midpoint23 = Vertex::Lerp(v2, v3, 0.5f);
				const Vertex midpoint31 = Vertex::Lerp(v3, v1, 0.5f);

				const Vertex tri1[3] = { v1, midpoint12, midpoint31 };
				const Vertex tri2[3] = { v2, midpoint23, midpoint12 };
				const Vertex tri3[3] = { v3, midpoint31, midpoint23 };
				const Vertex tri4[3] = { midpoint12, midpoint23, midpoint31 };

				AddTriangle(tri1, true, someVertices, someTriangles);
				AddTriangle(tri2, true, someVertices, someTriangles);
				AddTriangle(tri3, true, someVertices, someTriangles);
				AddTriangle(tri4, true, someVertices, someTriangles);
			}
		}*/

		void PopulateCapsule(std::vector<Vertex>& someVertices, std::vector<Triangle>& someTriangles)
		{
			PopulateQuad(someVertices, someTriangles);
		}

		void PopulateCube(std::vector<Vertex>& someVertices, std::vector<Triangle>& someTriangles)
		{
			Vertex* v = nullptr;

			// Z-
			v = &someVertices.emplace_back(); v->Position = {-1,  1, -1 }; v->UV = { 0, 1 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF00FFFF;
			v = &someVertices.emplace_back(); v->Position = { 1,  1, -1 }; v->UV = { 1, 1 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = {-1, -1, -1 }; v->UV = { 0, 0 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = {-1, -1, -1 }; v->UV = { 0, 0 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = { 1,  1, -1 }; v->UV = { 1, 1 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = { 1, -1, -1 }; v->UV = { 1, 0 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFF00FF;

			// Z+
			v = &someVertices.emplace_back(); v->Position = { 1,  1,  1 }; v->UV = { 0, 1 }; v->Normal = { 0, 0,  1 }; v->Tangent = { -1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF00FFFF;
			v = &someVertices.emplace_back(); v->Position = {-1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = { 0, 0,  1 }; v->Tangent = { -1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = { 1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = { 0, 0,  1 }; v->Tangent = { -1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = { 1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = { 0, 0,  1 }; v->Tangent = { -1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = {-1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = { 0, 0,  1 }; v->Tangent = { -1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = {-1, -1,  1 }; v->UV = { 1, 0 }; v->Normal = { 0, 0,  1 }; v->Tangent = { -1, 0, 0 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFF00FF;

			// X+
			v = &someVertices.emplace_back(); v->Position = { 1,  1, -1 }; v->UV = { 0, 1 }; v->Normal = { 1, 0, 0 }; v->Tangent = { 0, 0,  1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF00FFFF;
			v = &someVertices.emplace_back(); v->Position = { 1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = { 1, 0, 0 }; v->Tangent = { 0, 0,  1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = { 1, -1, -1 }; v->UV = { 0, 0 }; v->Normal = { 1, 0, 0 }; v->Tangent = { 0, 0,  1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = { 1, -1, -1 }; v->UV = { 0, 0 }; v->Normal = { 1, 0, 0 }; v->Tangent = { 0, 0,  1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = { 1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = { 1, 0, 0 }; v->Tangent = { 0, 0,  1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = { 1, -1,  1 }; v->UV = { 1, 0 }; v->Normal = { 1, 0, 0 }; v->Tangent = { 0, 0,  1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFF00FF;

			// X-
			v = &someVertices.emplace_back(); v->Position = {-1,  1,  1 }; v->UV = { 0, 1 }; v->Normal = { -1, 0, 0 }; v->Tangent = { 0, 0, -1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF00FFFF;
			v = &someVertices.emplace_back(); v->Position = {-1,  1, -1 }; v->UV = { 1, 1 }; v->Normal = { -1, 0, 0 }; v->Tangent = { 0, 0, -1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = {-1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = { -1, 0, 0 }; v->Tangent = { 0, 0, -1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = {-1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = { -1, 0, 0 }; v->Tangent = { 0, 0, -1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = {-1,  1, -1 }; v->UV = { 1, 1 }; v->Normal = { -1, 0, 0 }; v->Tangent = { 0, 0, -1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = {-1, -1, -1 }; v->UV = { 1, 0 }; v->Normal = { -1, 0, 0 }; v->Tangent = { 0, 0, -1 }; v->Binormal = { 0,  1, 0 }; v->Color = 0xFFFF00FF;

			// Y+
			v = &someVertices.emplace_back(); v->Position = {-1,  1,  1 }; v->UV = { 0, 1 }; v->Normal = { 0,  1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0,  1 }; v->Color = 0xFF00FFFF;
			v = &someVertices.emplace_back(); v->Position = { 1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = { 0,  1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0,  1 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = {-1,  1, -1 }; v->UV = { 0, 0 }; v->Normal = { 0,  1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0,  1 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = {-1,  1, -1 }; v->UV = { 0, 0 }; v->Normal = { 0,  1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0,  1 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = { 1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = { 0,  1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0,  1 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = { 1,  1, -1 }; v->UV = { 1, 0 }; v->Normal = { 0,  1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0,  1 }; v->Color = 0xFFFF00FF;

			// Y-
			v = &someVertices.emplace_back(); v->Position = {-1, -1, -1 }; v->UV = { 0, 1 }; v->Normal = { 0, -1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0, -1 }; v->Color = 0xFF00FFFF;
			v = &someVertices.emplace_back(); v->Position = { 1, -1, -1 }; v->UV = { 1, 1 }; v->Normal = { 0, -1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0, -1 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = {-1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = { 0, -1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0, -1 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = {-1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = { 0, -1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0, -1 }; v->Color = 0xFF0000FF;
			v = &someVertices.emplace_back(); v->Position = { 1, -1, -1 }; v->UV = { 1, 1 }; v->Normal = { 0, -1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0, -1 }; v->Color = 0xFFFFFFFF;
			v = &someVertices.emplace_back(); v->Position = { 1, -1,  1 }; v->UV = { 1, 0 }; v->Normal = { 0, -1, 0 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 0, -1 }; v->Color = 0xFFFF00FF;

			GenerateNormals_Flat(someVertices, someTriangles);
		}

		void PopulateCylinder(std::vector<Vertex>& someVertices, std::vector<Triangle>& someTriangles)
		{
			PopulateQuad(someVertices, someTriangles);
		}

		void PopulateDisc(std::vector<Vertex>& someVertices, std::vector<Triangle>& someTriangles)
		{
			const std::size_t first = someVertices.size();

			{
				Vertex* v = nullptr;
				v = &someVertices.emplace_back(); v->Position = { 0, 0, 0 }; v->UV = { 0, 0 };

				static constexpr std::size_t numSegments = 24;
				for (std::size_t i = 0; i < numSegments; i++)
				{
					const float theta = RoseCommon::Math::TwoPi<float> * static_cast<float>(i) / static_cast<float>(numSegments);

					const float x = cosf(theta);
					const float y = sinf(theta);
					v = &someVertices.emplace_back(); v->Position = { x, 0, -y }; v->UV = { 0, 0 }; v->Color = 0xFF0000FF;
				}
			}

			for (std::size_t i = 1; i < someVertices.size() - 1; ++i)
				someTriangles.emplace_back(Triangle { static_cast<std::uint32_t>(first + 0), static_cast<std::uint32_t>(first + i), static_cast<std::uint32_t>(first + i + 1) });
			someTriangles.emplace_back(Triangle { static_cast<std::uint32_t>(first + 0), static_cast<std::uint32_t>(first + someVertices.size() - 1), static_cast<std::uint32_t>(first + 1) });

			GenerateNormals_Flat(someVertices, someTriangles);
		}

		void PopulatePlane(std::vector<Vertex>& someVertices, std::vector<Triangle>& someTriangles)
		{
			{
				Vertex* v = nullptr;
				v = &someVertices.emplace_back(); v->Position = {-1, 0, 1 }; v->UV = { 0, 1 }; v->Color = 0xFF00FFFF;
				v = &someVertices.emplace_back(); v->Position = { 1, 0, 1 }; v->UV = { 1, 1 }; v->Color = 0xFFFFFFFF;
				v = &someVertices.emplace_back(); v->Position = {-1, 0,-1 }; v->UV = { 0, 0 }; v->Color = 0xFF0000FF;
				v = &someVertices.emplace_back(); v->Position = { 1, 0,-1 }; v->UV = { 1, 0 }; v->Color = 0xFFFF00FF;
			}

			someTriangles.emplace_back(Triangle { 0, 1, 2 });
			someTriangles.emplace_back(Triangle { 2, 1, 3 });

			/*Subdivide(someVertices, someTriangles);
			Subdivide(someVertices, someTriangles);*/

			GenerateNormals_Flat(someVertices, someTriangles);
		}

		void PopulateSphere(std::vector<Vertex>& someVertices, std::vector<Triangle>& someTriangles)
		{
			PopulateQuad(someVertices, someTriangles);
		}

		/*void PopulateIcosphere(std::vector<Vertex>& someVertices, std::vector<Triangle>& someTriangles)
		{
			const float t = (1.0f + RoseCommon::Math::Squareroot(5.0f)) / 2.0f;

			{
				Vertex* v = nullptr;
				v = &someVertices.emplace_back(); v->Position = { -1, t, 0 }; v->UV = { 0, 0 };
				v = &someVertices.emplace_back(); v->Position = { 1, t, 0 }; v->UV = { 0, 0 };
				v = &someVertices.emplace_back(); v->Position = { -1,-t, 0 }; v->UV = { 0, 0 };
				v = &someVertices.emplace_back(); v->Position = { 1,-t, 0 }; v->UV = { 0, 0 };

				v = &someVertices.emplace_back(); v->Position = { 0,-1, t }; v->UV = { 0, 0 };
				v = &someVertices.emplace_back(); v->Position = { 0, 1, t }; v->UV = { 0, 0 };
				v = &someVertices.emplace_back(); v->Position = { 0,-1,-t }; v->UV = { 0, 0 };
				v = &someVertices.emplace_back(); v->Position = { 0, 1,-t }; v->UV = { 0, 0 };

				v = &someVertices.emplace_back(); v->Position = { t, 0,-1 }; v->UV = { 0, 0 };
				v = &someVertices.emplace_back(); v->Position = { t, 0, 1 }; v->UV = { 0, 0 };
				v = &someVertices.emplace_back(); v->Position = { -t, 0,-1 }; v->UV = { 0, 0 };
				v = &someVertices.emplace_back(); v->Position = { -t, 0, 1 }; v->UV = { 0, 0 };
			}

			{
				someTriangles.emplace_back(Triangle { 0, 11,  5 });
				someTriangles.emplace_back(Triangle { 0,  5,  1 });
				someTriangles.emplace_back(Triangle { 0,  1,  7 });
				someTriangles.emplace_back(Triangle { 0,  7, 10 });
				someTriangles.emplace_back(Triangle { 0, 10, 11 });

				someTriangles.emplace_back(Triangle { 1,  5,  9 });
				someTriangles.emplace_back(Triangle { 5, 11,  4 });
				someTriangles.emplace_back(Triangle { 11, 10,  2 });
				someTriangles.emplace_back(Triangle { 10,  7,  6 });
				someTriangles.emplace_back(Triangle { 7,  1,  8 });

				someTriangles.emplace_back(Triangle { 3,  9,  4 });
				someTriangles.emplace_back(Triangle { 3,  4,  2 });
				someTriangles.emplace_back(Triangle { 3,  2,  6 });
				someTriangles.emplace_back(Triangle { 3,  6,  8 });
				someTriangles.emplace_back(Triangle { 3,  8,  9 });

				someTriangles.emplace_back(Triangle { 4,  9,  5 });
				someTriangles.emplace_back(Triangle { 2,  4, 11 });
				someTriangles.emplace_back(Triangle { 6,  2, 10 });
				someTriangles.emplace_back(Triangle { 8,  6,  7 });
				someTriangles.emplace_back(Triangle { 9,  8,  1 });
			}

			Subdivide(someVertices, someTriangles);
			Subdivide(someVertices, someTriangles);

			for (Vertex& v : someVertices)
				v.Position.Normalize();

			GenerateNormals_Smooth(someVertices, someTriangles);
		}*/

		void PopulateQuad(std::vector<Vertex>& someVertices, std::vector<Triangle>& someTriangles)
		{
			{
				Vertex* v = nullptr;
				v = &someVertices.emplace_back(); v->Position = {-1, 1, 0 }; v->UV = { 0, 1 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 1, 0 }; v->Color = 0xFF00FFFF;
				v = &someVertices.emplace_back(); v->Position = { 1, 1, 0 }; v->UV = { 1, 1 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 1, 0 }; v->Color = 0xFFFFFFFF;
				v = &someVertices.emplace_back(); v->Position = {-1,-1, 0 }; v->UV = { 0, 0 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 1, 0 }; v->Color = 0xFF0000FF;
				v = &someVertices.emplace_back(); v->Position = { 1,-1, 0 }; v->UV = { 1, 0 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 1, 0 }; v->Color = 0xFFFF00FF;
			}

			someTriangles.emplace_back(Triangle { 0, 1, 2 });
			someTriangles.emplace_back(Triangle { 2, 1, 3 });
		}

	}

	void Mesh::SetFromPrimitive(MeshPrimitiveType aPrimitive)
	{
		std::vector<Vertex> vertices;
		std::vector<Triangle> triangles;

		switch (aPrimitive)
		{
		case MeshPrimitiveType::Capsule:
			MakeMeshPrimitives::PopulateCapsule(vertices, triangles);
			break;
		case MeshPrimitiveType::Cube:
			MakeMeshPrimitives::PopulateCube(vertices, triangles);
			break;
		case MeshPrimitiveType::Cylinder:
			MakeMeshPrimitives::PopulateCylinder(vertices, triangles);
			break;
		case MeshPrimitiveType::Disc:
			MakeMeshPrimitives::PopulateDisc(vertices, triangles);
			break;
		case MeshPrimitiveType::Plane:
			MakeMeshPrimitives::PopulatePlane(vertices, triangles);
			break;
		case MeshPrimitiveType::Sphere:
			MakeMeshPrimitives::PopulateSphere(vertices, triangles);
			break;
		case MeshPrimitiveType::Icosphere:
			//MakeMeshPrimitives::PopulateIcosphere(vertices, triangles);
			break;
		case MeshPrimitiveType::Quad:
			MakeMeshPrimitives::PopulateQuad(vertices, triangles);
			break;
		}

		SetFromList(vertices, triangles);
	}
}

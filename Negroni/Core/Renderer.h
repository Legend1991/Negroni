#pragma once

#include "../Game.h"

namespace Core
{
	class Renderer
	{
	public:
		virtual void Resize(size_t width, size_t height) = 0;
		virtual void LoadMesh(const MeshRef& mesh) = 0;
		virtual void UnloadMesh(const MeshRef & mesh) = 0;
		virtual void Clear(const rgba color) = 0;
		virtual void Draw(const GameState&) = 0;
		virtual void Present() = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual void SetFXAA(bool enabled) = 0;
		virtual void Cleanup() = 0;

		u32 rendered = 0;
		u32 culled = 0;
	};
}

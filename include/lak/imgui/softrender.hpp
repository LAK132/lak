#ifndef LAK_IMGUI_SOFTRENDER_HPP
#define LAK_IMGUI_SOFTRENDER_HPP

#include "lak/softrender/softrender.hpp"

#include <imgui.h>

bool ImGui_ImplSoftrender_Init(texture_base_t *screen);
void ImGui_ImplSoftrender_Shutdown();
void ImGui_ImplSoftrender_NewFrame();
void ImGui_ImplSoftrender_RenderDrawData(ImDrawData *draw_data);

template<typename POS, typename SCREEN>
void renderCommand(texture_t<SCREEN> &screen,
                   const texture_base_t *texture,
                   const ImDrawVert *vtx_buffer,
                   const ImDrawIdx *idx_buffer,
                   const ImDrawCmd &pcmd)
{
	const clip_t<POS> clip = {{inl_max((POS)pcmd.ClipRect.x, (POS)0),
	                           inl_min((POS)pcmd.ClipRect.z, (POS)screen.w)},
	                          {inl_max((POS)pcmd.ClipRect.y, (POS)0),
	                           inl_min((POS)pcmd.ClipRect.w, (POS)screen.h)}};

	for (unsigned int i = 0; i < pcmd.ElemCount; i += 3)
	{
		const ImDrawVert *verts[] = {&vtx_buffer[idx_buffer[i]],
		                             &vtx_buffer[idx_buffer[i + 1]],
		                             &vtx_buffer[idx_buffer[i + 2]]};

		if (i < pcmd.ElemCount - 3)
		{
			ImVec2 tlpos = verts[0]->pos;
			ImVec2 brpos = verts[0]->pos;
			ImVec2 tluv  = verts[0]->uv;
			ImVec2 bruv  = verts[0]->uv;
			for (int v = 1; v < 3; v++)
			{
				if (verts[v]->pos.x < tlpos.x)
				{
					tlpos.x = verts[v]->pos.x;
					tluv.x  = verts[v]->uv.x;
				}
				else if (verts[v]->pos.x > brpos.x)
				{
					brpos.x = verts[v]->pos.x;
					bruv.x  = verts[v]->uv.x;
				}
				if (verts[v]->pos.y < tlpos.y)
				{
					tlpos.y = verts[v]->pos.y;
					tluv.y  = verts[v]->uv.y;
				}
				else if (verts[v]->pos.y > brpos.y)
				{
					brpos.y = verts[v]->pos.y;
					bruv.y  = verts[v]->uv.y;
				}
			}

			const ImDrawVert *nextVerts[] = {&vtx_buffer[idx_buffer[i + 3]],
			                                 &vtx_buffer[idx_buffer[i + 4]],
			                                 &vtx_buffer[idx_buffer[i + 5]]};

			bool isRect = true;
			for (int v = 0; v < 3; v++)
			{
				if (((nextVerts[v]->pos.x != tlpos.x) &&
				     (nextVerts[v]->pos.x != brpos.x)) ||
				    ((nextVerts[v]->pos.y != tlpos.y) &&
				     (nextVerts[v]->pos.y != brpos.y)) ||
				    ((nextVerts[v]->uv.x != tluv.x) &&
				     (nextVerts[v]->uv.x != bruv.x)) ||
				    ((nextVerts[v]->uv.y != tluv.y) && (nextVerts[v]->uv.y != bruv.y)))
				{
					isRect = false;
					break;
				}
			}

			if (isRect)
			{
				rectangle_t<POS, SCREEN> quad;
				quad.p1.x = static_cast<POS>(tlpos.x);
				quad.p1.y = static_cast<POS>(tlpos.y);
				quad.p2.x = static_cast<POS>(brpos.x);
				quad.p2.y = static_cast<POS>(brpos.y);
				quad.p1.u = tluv.x;
				quad.p1.v = tluv.y;
				quad.p2.u = bruv.x;
				quad.p2.v = bruv.y;
				quad.p1.c =
				  color32_t(static_cast<uint8_t>(verts[0]->col >> IM_COL32_R_SHIFT),
				            static_cast<uint8_t>(verts[0]->col >> IM_COL32_G_SHIFT),
				            static_cast<uint8_t>(verts[0]->col >> IM_COL32_B_SHIFT),
				            static_cast<uint8_t>(verts[0]->col >> IM_COL32_A_SHIFT));
				quad.p2.c = quad.p1.c;

				const bool noUV = (quad.p1.u == quad.p2.u) && (quad.p1.v == quad.p2.v);
				const bool alphaBlend = true;

				renderQuad(screen, noUV ? nullptr : texture, clip, quad, alphaBlend);

				i += 3;
				continue;
			}
		}

		triangle_t<POS, SCREEN> tri;
		// triangle_t<POS, color32_t> tri;
		tri.p1.x = static_cast<POS>(verts[0]->pos.x);
		tri.p1.y = static_cast<POS>(verts[0]->pos.y);
		tri.p1.u = verts[0]->uv.x;
		tri.p1.v = verts[0]->uv.y;
		tri.p1.c =
		  color32_t(static_cast<uint8_t>(verts[0]->col >> IM_COL32_R_SHIFT),
		            static_cast<uint8_t>(verts[0]->col >> IM_COL32_G_SHIFT),
		            static_cast<uint8_t>(verts[0]->col >> IM_COL32_B_SHIFT),
		            static_cast<uint8_t>(verts[0]->col >> IM_COL32_A_SHIFT));

		tri.p2.x = static_cast<POS>(verts[1]->pos.x);
		tri.p2.y = static_cast<POS>(verts[1]->pos.y);
		tri.p2.u = verts[1]->uv.x;
		tri.p2.v = verts[1]->uv.y;
		tri.p2.c =
		  color32_t(static_cast<uint8_t>(verts[1]->col >> IM_COL32_R_SHIFT),
		            static_cast<uint8_t>(verts[1]->col >> IM_COL32_G_SHIFT),
		            static_cast<uint8_t>(verts[1]->col >> IM_COL32_B_SHIFT),
		            static_cast<uint8_t>(verts[1]->col >> IM_COL32_A_SHIFT));

		tri.p3.x = static_cast<POS>(verts[2]->pos.x);
		tri.p3.y = static_cast<POS>(verts[2]->pos.y);
		tri.p3.u = verts[2]->uv.x;
		tri.p3.v = verts[2]->uv.y;
		tri.p3.c =
		  color32_t(static_cast<uint8_t>(verts[2]->col >> IM_COL32_R_SHIFT),
		            static_cast<uint8_t>(verts[2]->col >> IM_COL32_G_SHIFT),
		            static_cast<uint8_t>(verts[2]->col >> IM_COL32_B_SHIFT),
		            static_cast<uint8_t>(verts[2]->col >> IM_COL32_A_SHIFT));

		// Make sure the winding order is correct.
		if (halfspace(edge_t<POS>{point_t<POS>{tri.p1.x, tri.p1.y},
		                          point_t<POS>{tri.p2.x, tri.p2.y}},
		              point_t<POS>{tri.p3.x, tri.p3.y}) > 0)
			swap(&tri.p2, &tri.p3);

		const bool noUV = (tri.p1.u == tri.p2.u) && (tri.p1.u == tri.p3.u) &&
		                  (tri.p1.v == tri.p2.v) && (tri.p1.v == tri.p3.v);
		const bool flatCol =
		  noUV || ((tri.p1.c == tri.p2.c) && (tri.p1.c == tri.p3.c));
		const bool alphaBlend = true;

		renderTri(
		  screen, noUV ? nullptr : texture, clip, tri, !flatCol, alphaBlend);
	}
}

template<typename POS, typename SCREEN>
void renderDrawLists(ImDrawData *drawData, texture_t<SCREEN> &screen)
{
	ImGuiIO &io  = ImGui::GetIO();
	int fbWidth  = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fbHeight = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fbWidth == 0 || fbHeight == 0) return;
	drawData->ScaleClipRects(io.DisplayFramebufferScale);

	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList *cmdList    = drawData->CmdLists[n];
		const ImDrawVert *vtx_buffer = cmdList->VtxBuffer.Data;
		const ImDrawIdx *idx_buffer  = cmdList->IdxBuffer.Data;

		for (int cmdi = 0; cmdi < cmdList->CmdBuffer.Size; cmdi++)
		{
			const ImDrawCmd &pcmd = cmdList->CmdBuffer[cmdi];
			if (pcmd.UserCallback)
			{
				pcmd.UserCallback(cmdList, &pcmd);
			}
			else
			{
				renderCommand<POS>(
				  screen,
				  reinterpret_cast<const texture_base_t *>(pcmd.GetTexID()),
				  vtx_buffer,
				  idx_buffer,
				  pcmd);
			}
			idx_buffer += pcmd.ElemCount;
		}
	}
}

#endif

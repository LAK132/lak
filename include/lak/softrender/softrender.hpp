#ifndef LAK_SOFTRENDER_SOFTRENDER_HPP
#define LAK_SOFTRENDER_SOFTRENDER_HPP

#include "lak/softrender/barycentric.hpp"
#include "lak/softrender/color.hpp"
#include "lak/softrender/defines.hpp"
#include "lak/softrender/texture.hpp"
#include "lak/softrender/utils.hpp"

template<typename POS, typename SCREEN, typename TEXTURE, typename COLOR>
void renderQuadCore(texture_t<SCREEN> &screen,
                    const texture_t<TEXTURE> &tex,
                    const clip_t<POS> &clip,
                    const rectangle_t<POS, COLOR> &quad,
                    const bool alphaBlend)
{
	if ((quad.p2.x < clip.x.min) || (quad.p2.y < clip.y.min) ||
	    (quad.p1.x >= clip.x.max) || (quad.p1.y >= clip.y.max))
		return;

	const range_t<POS> qx = {quad.p1.x, quad.p2.x};
	const range_t<POS> qy = {quad.p1.y, quad.p2.y};

	const range_t<float> qu = {inl_max((float)quad.p1.u * tex.w, 0.0f),
	                           inl_min((float)quad.p2.u * tex.w, (float)tex.w)};

	const range_t<float> qv = {inl_max((float)quad.p1.v * tex.h, 0.0f),
	                           inl_min((float)quad.p2.v * tex.h, (float)tex.h)};

	const range_t<POS> rx = inl_min(qx, clip.x);
	const range_t<POS> ry = inl_min(qy, clip.y);

	const float duDx = (qu.max - qu.min) / (qx.max - qx.min);
	const float dvDy = (qv.max - qv.min) / (qy.max - qy.min);

	const float xoffset = (float)rx.min - (float)qx.min;
	const float yoffset = (float)ry.min - (float)qy.min;

	const float startu = qu.min + (xoffset > 0 ? duDx * xoffset : 0);
	const float startv = qv.min + (yoffset > 0 ? dvDy * yoffset : 0);

	bool blit = ((duDx == 1.0f) && (dvDy == 1.0f));

	if (blit)
	{
		const POS u = static_cast<POS>(startu - rx.min);
		const POS v = static_cast<POS>(startv - ry.min);
		if (alphaBlend)
		{
			for (POS y = ry.min; y < ry.max; ++y)
			{
				for (POS x = rx.min; x < rx.max; ++x)
				{
					screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) %=
					  quad.p1.c *
					  tex.at(static_cast<size_t>(x + u), static_cast<size_t>(y + v));
				}
			}
		}
		else
		{
			for (POS y = ry.min; y < ry.max; ++y)
			{
				for (POS x = rx.min; x < rx.max; ++x)
				{
					screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) =
					  quad.p1.c *
					  tex.at(static_cast<size_t>(x + u), static_cast<size_t>(y + v));
				}
			}
		}
	}
	else
	{
		if (alphaBlend)
		{
			float v = startv;
			POS y   = ry.min;
			while (y < ry.max)
			{
				float u = startu;
				POS x   = rx.min;
				while (x < rx.max)
				{
					screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) %=
					  quad.p1.c * tex.at(static_cast<size_t>(u), static_cast<size_t>(v));
					++x;
					u += duDx;
				}
				++y;
				v += dvDy;
			}
		}
		else
		{
			float v = startv;
			POS y   = ry.min;
			while (y < ry.max)
			{
				float u = startu;
				POS x   = rx.min;
				while (x < rx.max)
				{
					screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) =
					  quad.p1.c * tex.at(static_cast<size_t>(u), static_cast<size_t>(v));
					++x;
					u += duDx;
				}
				++y;
				v += dvDy;
			}
		}
	}
}

template<typename POS, typename SCREEN, typename COLOR>
void renderQuadCore(texture_t<SCREEN> &screen,
                    const clip_t<POS> &clip,
                    const rectangle_t<POS, COLOR> &quad,
                    const bool alphaBlend)
{
	if ((quad.p2.x < clip.x.min) || (quad.p2.y < clip.y.min) ||
	    (quad.p1.x >= clip.x.max) || (quad.p1.y >= clip.y.max))
		return;

	const range_t<POS> rx = inl_min({quad.p1.x, quad.p2.x}, clip.x);
	const range_t<POS> ry = inl_min({quad.p1.y, quad.p2.y}, clip.y);

	if (alphaBlend)
	{
		for (POS y = ry.min; y < ry.max; ++y)
		{
			for (POS x = rx.min; x < rx.max; ++x)
			{
				screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) %= quad.p1.c;
			}
		}
	}
	else
	{
		for (POS y = ry.min; y < ry.max; ++y)
		{
			for (POS x = rx.min; x < rx.max; ++x)
			{
				screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) = quad.p1.c;
			}
		}
	}
}

template<typename POS, typename SCREEN, typename COLOR>
void renderQuad(texture_t<SCREEN> &screen,
                const texture_base_t *tex,
                const clip_t<POS> &clip,
                const rectangle_t<POS, COLOR> &quad,
                const bool alphaBlend)
{
	switch (tex == nullptr ? texture_type_t::NONE : tex->type)
	{
		case texture_type_t::ALPHA8:
			renderQuadCore(screen,
			               *reinterpret_cast<const texture_alpha8_t *>(tex),
			               clip,
			               quad,
			               alphaBlend);
			break;

		case texture_type_t::VALUE8:
			renderQuadCore(screen,
			               *reinterpret_cast<const texture_value8_t *>(tex),
			               clip,
			               quad,
			               alphaBlend);
			break;

		case texture_type_t::COLOR16:
			renderQuadCore(screen,
			               *reinterpret_cast<const texture_color16_t *>(tex),
			               clip,
			               quad,
			               alphaBlend);
			break;

		case texture_type_t::COLOR24:
			renderQuadCore(screen,
			               *reinterpret_cast<const texture_color24_t *>(tex),
			               clip,
			               quad,
			               alphaBlend);
			break;

		case texture_type_t::COLOR32:
			renderQuadCore(screen,
			               *reinterpret_cast<const texture_color32_t *>(tex),
			               clip,
			               quad,
			               alphaBlend);
			break;

		default:
			renderQuadCore(screen, clip, quad, alphaBlend);
			break;
	}
}

template<typename POS, typename COLOR, typename TEXTURE>
point_t<POS> texCoord(const pixel_t<POS, COLOR> &p,
                      const texture_t<TEXTURE> &tex)
{
	return {mod((POS)((p.u * tex.w) + 0.5f), (POS)tex.w),
	        mod((POS)((p.v * tex.h) + 0.5f), (POS)tex.h)};
}

template<typename POS, typename SCREEN, typename TEXTURE, typename COLOR>
void renderTriCore(texture_t<SCREEN> &screen,
                   const texture_t<TEXTURE> &tex,
                   const clip_t<POS> &clip,
                   const range_t<POS> &rY,
                   const range_t<POS> &rX,
                   const bary_t<POS, COLOR> &bary,
                   const bool alphaBlend)
{
	const range_t<size_t> ry = {(size_t)inl_max(rY.min, clip.y.min),
	                            (size_t)inl_min(rY.max, clip.y.max)};
	const range_t<size_t> rx = {(size_t)inl_max(rX.min, clip.x.min),
	                            (size_t)inl_min(rX.max, clip.x.max)};
	if (alphaBlend)
	{
		for (size_t y = ry.min; y < ry.max; ++y)
		{
			for (size_t x = rx.min; x < rx.max; ++x)
			{
				if (!triangle_hit(bary, x, y)) continue;
				pixel_t<POS, COLOR> p;
				p.x = static_cast<POS>(x);
				p.y = static_cast<POS>(y);
				barycentricUV(p, bary);

				point_t<POS> coord = texCoord(p, tex);

				screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) %=
				  bary.a.c *
				  tex.at(static_cast<size_t>(coord.x), static_cast<size_t>(coord.y));
			}
		}
	}
	else
	{
		for (size_t y = ry.min; y < ry.max; ++y)
		{
			for (size_t x = rx.min; x < rx.max; ++x)
			{
				if (!triangle_hit(bary, x, y)) continue;
				pixel_t<POS, COLOR> p;
				p.x = static_cast<POS>(x);
				p.y = static_cast<POS>(y);
				barycentricUV(p, bary);

				point_t<POS> coord = texCoord(p, tex);

				screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) =
				  bary.a.c *
				  tex.at(static_cast<size_t>(coord.x), static_cast<size_t>(coord.y));
			}
		}
	}
}

template<typename POS, typename SCREEN, typename COLOR>
void renderTriCore(texture_t<SCREEN> &screen,
                   const clip_t<POS> &clip,
                   const range_t<POS> &rY,
                   const range_t<POS> &rX,
                   const bary_t<POS, COLOR> &bary,
                   const bool uvBlend,
                   const bool alphaBlend)
{
	const range_t<size_t> ry = {(size_t)inl_max(rY.min, clip.y.min),
	                            (size_t)inl_min(rY.max, clip.y.max)};
	const range_t<size_t> rx = {(size_t)inl_max(rX.min, clip.x.min),
	                            (size_t)inl_min(rX.max, clip.x.max)};

	if (uvBlend)
	{
		if (alphaBlend)
		{
			for (size_t y = ry.min; y < ry.max; ++y)
			{
				for (size_t x = rx.min; x < rx.max; ++x)
				{
					if (!triangle_hit(bary, x, y)) continue;
					pixel_t<POS, COLOR> p;
					p.x = static_cast<POS>(x);
					p.y = static_cast<POS>(y);
					barycentricCol(p, bary);
					screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) %= p.c;
				}
			}
		}
		else
		{
			for (size_t y = ry.min; y < ry.max; ++y)
			{
				for (size_t x = rx.min; x < rx.max; ++x)
				{
					if (!triangle_hit(bary, x, y)) continue;
					pixel_t<POS, COLOR> p;
					p.x = static_cast<POS>(x);
					p.y = static_cast<POS>(y);
					barycentricCol(p, bary);
					screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) = p.c;
				}
			}
		}
	}
	else
	{
		if (alphaBlend)
		{
			for (size_t y = ry.min; y < ry.max; ++y)
			{
				for (size_t x = rx.min; x < rx.max; ++x)
				{
					if (!triangle_hit(bary, x, y)) continue;
					screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) %=
					  bary.a.c;
				}
			}
		}
		else
		{
			for (size_t y = ry.min; y < ry.max; ++y)
			{
				for (size_t x = rx.min; x < rx.max; ++x)
				{
					if (!triangle_hit(bary, x, y)) continue;
					screen.at(static_cast<size_t>(x), static_cast<size_t>(y)) = bary.a.c;
				}
			}
		}
	}
}

template<typename POS, typename SCREEN, typename COLOR>
void renderTri(texture_t<SCREEN> &screen,
               const texture_base_t *tex,
               const clip_t<POS> &clip,
               const range_t<POS> &rY,
               const range_t<POS> &rX,
               const bary_t<POS, COLOR> &bary,
               const bool uvBlend,
               const bool alphaBlend)
{
	switch (tex == nullptr ? texture_type_t::NONE : tex->type)
	{
		case texture_type_t::ALPHA8:
			renderTriCore(screen,
			              *reinterpret_cast<const texture_alpha8_t *>(tex),
			              clip,
			              rY,
			              rX,
			              bary,
			              alphaBlend);
			break;

		case texture_type_t::VALUE8:
			renderTriCore(screen,
			              *reinterpret_cast<const texture_value8_t *>(tex),
			              clip,
			              rY,
			              rX,
			              bary,
			              alphaBlend);
			break;

		case texture_type_t::COLOR16:
			renderTriCore(screen,
			              *reinterpret_cast<const texture_color16_t *>(tex),
			              clip,
			              rY,
			              rX,
			              bary,
			              alphaBlend);
			break;

		case texture_type_t::COLOR24:
			renderTriCore(screen,
			              *reinterpret_cast<const texture_color24_t *>(tex),
			              clip,
			              rY,
			              rX,
			              bary,
			              alphaBlend);
			break;

		case texture_type_t::COLOR32:
			renderTriCore(screen,
			              *reinterpret_cast<const texture_color32_t *>(tex),
			              clip,
			              rY,
			              rX,
			              bary,
			              alphaBlend);
			break;

		default:
			renderTriCore(screen, clip, rY, rX, bary, uvBlend, alphaBlend);
			break;
	}
}

template<typename POS, typename SCREEN, typename COLOR>
void renderTri(texture_t<SCREEN> &screen,
               const texture_base_t *tex,
               const clip_t<POS> &clip,
               triangle_t<POS, COLOR> &tri,
               const bool uvBlend,
               const bool alphaBlend)
{
	renderTri(screen,
	          tex,
	          clip,
	          {inl_min(inl_min(tri.p1.y, tri.p2.y), tri.p3.y),
	           inl_max(inl_max(tri.p1.y, tri.p2.y), tri.p3.y) + 1},
	          {inl_min(inl_min(tri.p1.x, tri.p2.x), tri.p3.x),
	           inl_max(inl_max(tri.p1.x, tri.p2.x), tri.p3.x) + 1},
	          baryPre(tri.p1, tri.p2, tri.p3),
	          uvBlend,
	          alphaBlend);
}

#endif

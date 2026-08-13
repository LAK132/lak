#include "lak/vec.hpp"

#include "lak/test.hpp"

BEGIN_TEST(matrix_determinant)
{
	{
		const lak::mat2f_t mat{
		  {1.f, 0.f},
		  {0.f, 1.f},
		};
		const auto mat_det = det(mat);
		ASSERT_CLOSE(mat_det, 1.f);
	}

	{
		const lak::mat2f_t mat{
		  {0.f, 1.f},
		  {1.f, 0.f},
		};
		const auto mat_det = det(mat);
		ASSERT_CLOSE(mat_det, -1.f);
	}

	{
		const lak::mat2f_t mat{
		  {1.f, 2.f},
		  {3.f, 4.f},
		};
		const auto mat_det = det(mat);
		ASSERT_CLOSE(mat_det, -2.f);
	}

	{
		const lak::mat2f_t mat{
		  {2.f, 3.f},
		  {5.f, 6.f},
		};
		const auto mat_det = det(mat);
		ASSERT_CLOSE(mat_det, -3.f);
	}

	{
		const lak::mat2f_t mat{
		  {3.f, 1.f},
		  {6.f, 4.f},
		};
		const auto mat_det = det(mat);
		ASSERT_CLOSE(mat_det, 6.f);
	}

	{
		const lak::mat2f_t mat{
		  {1.f, 2.f},
		  {4.f, 5.f},
		};
		const auto mat_det = det(mat);
		ASSERT_CLOSE(mat_det, -3.f);
	}

	{
		const lak::mat3f_t mat{
		  {1.f, 0.f, 0.f},
		  {0.f, 1.f, 0.f},
		  {0.f, 0.f, 1.f},
		};
		const auto mat_det = det(mat);
		ASSERT_CLOSE(mat_det, 1.f);
	}

	{
		const lak::mat3f_t mat{
		  {1.f, 2.f, 3.f},
		  {4.f, 5.f, 6.f},
		  {7.f, 8.f, 3.f},
		};
		const auto mat_det = det(mat);
		ASSERT_CLOSE(mat_det, 18.f);
	}

	{
		const lak::mat4f_t mat{
		  {1.f, 0.f, 0.f, 0.f},
		  {0.f, 1.f, 0.f, 0.f},
		  {0.f, 0.f, 1.f, 0.f},
		  {0.f, 0.f, 0.f, 1.f},
		};
		const auto mat_det = det(mat);
		ASSERT_CLOSE(mat_det, 1.f);
	}

	{
		const lak::mat4f_t mat{
		  {2.f, 1.f, 3.f, 4.f},
		  {0.f, -1.f, 2.f, 1.f},
		  {3.f, 2.f, 0.f, 5.f},
		  {-1.f, 3.f, 2.f, 1.f},
		};
		const auto mat_det = det(mat);
		ASSERT_CLOSE(mat_det, 35.f);
	}

	return 0;
}
END_TEST()

BEGIN_TEST(matrix_inverse)
{
	{
		const lak::mat2f_t mat{
		  {1.f, 0.f},
		  {0.f, 1.f},
		};
		const auto inv_mat = inverse(mat);
		ASSERT_CLOSE(inv_mat.x.x, mat.x.x);
		ASSERT_CLOSE(inv_mat.x.y, mat.x.y);

		ASSERT_CLOSE(inv_mat.y.x, mat.y.x);
		ASSERT_CLOSE(inv_mat.y.y, mat.y.y);
	}

	{
		const lak::mat3f_t mat{
		  {1.f, 0.f, 0.f},
		  {0.f, 1.f, 0.f},
		  {0.f, 0.f, 1.f},
		};
		const auto inv_mat = inverse(mat);
		ASSERT_CLOSE(inv_mat.x.x, mat.x.x);
		ASSERT_CLOSE(inv_mat.x.y, mat.x.y);
		ASSERT_CLOSE(inv_mat.x.z, mat.x.z);

		ASSERT_CLOSE(inv_mat.y.x, mat.y.x);
		ASSERT_CLOSE(inv_mat.y.y, mat.y.y);
		ASSERT_CLOSE(inv_mat.y.z, mat.y.z);

		ASSERT_CLOSE(inv_mat.z.x, mat.z.x);
		ASSERT_CLOSE(inv_mat.z.y, mat.z.y);
		ASSERT_CLOSE(inv_mat.z.z, mat.z.z);
	}

	{
		const lak::mat3f_t mat{
		  {2.f, 0.f, 0.f},
		  {0.f, 2.f, 0.f},
		  {0.f, 0.f, 2.f},
		};
		const auto inv_mat = inverse(mat);
		ASSERT_CLOSE(inv_mat.x.x, 0.5f);
		ASSERT_CLOSE(inv_mat.x.y, 0.f);
		ASSERT_CLOSE(inv_mat.x.z, 0.f);

		ASSERT_CLOSE(inv_mat.y.x, 0.f);
		ASSERT_CLOSE(inv_mat.y.y, 0.5f);
		ASSERT_CLOSE(inv_mat.y.z, 0.f);

		ASSERT_CLOSE(inv_mat.z.x, 0.f);
		ASSERT_CLOSE(inv_mat.z.y, 0.f);
		ASSERT_CLOSE(inv_mat.z.z, 0.5f);
	}

	{
		const lak::mat3f_t mat{
		  {1.f, 3.f, 2.f},
		  {0.f, 1.f, 0.f},
		  {0.f, 0.f, 1.f},
		};
		const auto inv_mat = inverse(mat);
		ASSERT_CLOSE(inv_mat.x.x, 1.f);
		ASSERT_CLOSE(inv_mat.x.y, -3.f);
		ASSERT_CLOSE(inv_mat.x.z, -2.f);

		ASSERT_CLOSE(inv_mat.y.x, mat.y.x);
		ASSERT_CLOSE(inv_mat.y.y, mat.y.y);
		ASSERT_CLOSE(inv_mat.y.z, mat.y.z);

		ASSERT_CLOSE(inv_mat.z.x, mat.z.x);
		ASSERT_CLOSE(inv_mat.z.y, mat.z.y);
		ASSERT_CLOSE(inv_mat.z.z, mat.z.z);
	}

	{
		const lak::mat3f_t mat{
		  {3.f, 0.f, 2.f},
		  {2.f, 1.f, 0.f},
		  {1.f, 4.f, 2.f},
		};
		const auto inv_mat = inverse(mat);
		ASSERT_CLOSE(inv_mat.x.x, 1.f / 10.f);
		ASSERT_CLOSE(inv_mat.x.y, 2.f / 5.f);
		ASSERT_CLOSE(inv_mat.x.z, -1.f / 10.f);

		ASSERT_CLOSE(inv_mat.y.x, -1.f / 5.f);
		ASSERT_CLOSE(inv_mat.y.y, 1.f / 5.f);
		ASSERT_CLOSE(inv_mat.y.z, 1.f / 5.f);

		ASSERT_CLOSE(inv_mat.z.x, 7.f / 20.f);
		ASSERT_CLOSE(inv_mat.z.y, -3.f / 5.f);
		ASSERT_CLOSE(inv_mat.z.z, 3.f / 20.f);
	}

	{
		const lak::mat4f_t mat{
		  {1.f, 0.f, 0.f, 0.f},
		  {0.f, 1.f, 0.f, 0.f},
		  {0.f, 0.f, 1.f, 0.f},
		  {0.f, 0.f, 0.f, 1.f},
		};
		const auto inv_mat = inverse(mat);
		ASSERT_CLOSE(inv_mat.x.x, mat.x.x);
		ASSERT_CLOSE(inv_mat.x.y, mat.x.y);
		ASSERT_CLOSE(inv_mat.x.z, mat.x.z);
		ASSERT_CLOSE(inv_mat.x.w, mat.x.w);

		ASSERT_CLOSE(inv_mat.y.x, mat.y.x);
		ASSERT_CLOSE(inv_mat.y.y, mat.y.y);
		ASSERT_CLOSE(inv_mat.y.z, mat.y.z);
		ASSERT_CLOSE(inv_mat.y.w, mat.y.w);

		ASSERT_CLOSE(inv_mat.z.x, mat.z.x);
		ASSERT_CLOSE(inv_mat.z.y, mat.z.y);
		ASSERT_CLOSE(inv_mat.z.z, mat.z.z);
		ASSERT_CLOSE(inv_mat.z.w, mat.z.w);

		ASSERT_CLOSE(inv_mat.w.x, mat.w.x);
		ASSERT_CLOSE(inv_mat.w.y, mat.w.y);
		ASSERT_CLOSE(inv_mat.w.z, mat.w.z);
		ASSERT_CLOSE(inv_mat.w.w, mat.w.w);
	}

	{
		const lak::mat4f_t mat{
		  {2.f, 0.f, 0.f, 0.f},
		  {0.f, 2.f, 0.f, 0.f},
		  {0.f, 0.f, 2.f, 0.f},
		  {0.f, 0.f, 0.f, 2.f},
		};
		const auto inv_mat = inverse(mat);
		ASSERT_CLOSE(inv_mat.x.x, 0.5f);
		ASSERT_CLOSE(inv_mat.x.y, 0.f);
		ASSERT_CLOSE(inv_mat.x.z, 0.f);
		ASSERT_CLOSE(inv_mat.x.w, 0.f);

		ASSERT_CLOSE(inv_mat.y.x, 0.f);
		ASSERT_CLOSE(inv_mat.y.y, 0.5f);
		ASSERT_CLOSE(inv_mat.y.z, 0.f);
		ASSERT_CLOSE(inv_mat.y.w, 0.f);

		ASSERT_CLOSE(inv_mat.z.x, 0.f);
		ASSERT_CLOSE(inv_mat.z.y, 0.f);
		ASSERT_CLOSE(inv_mat.z.z, 0.5f);
		ASSERT_CLOSE(inv_mat.z.w, 0.f);

		ASSERT_CLOSE(inv_mat.w.x, 0.f);
		ASSERT_CLOSE(inv_mat.w.y, 0.f);
		ASSERT_CLOSE(inv_mat.w.z, 0.f);
		ASSERT_CLOSE(inv_mat.w.w, 0.5f);
	}

	{
		const lak::mat4f_t mat{
		  {1.f, 3.f, 2.f, 0.f},
		  {0.f, 1.f, 0.f, 0.f},
		  {0.f, 0.f, 1.f, 0.f},
		  {0.f, 0.f, 0.f, 1.f},
		};
		const auto inv_mat = inverse(mat);
		ASSERT_CLOSE(inv_mat.x.x, 1.f);
		ASSERT_CLOSE(inv_mat.x.y, -3.f);
		ASSERT_CLOSE(inv_mat.x.z, -2.f);
		ASSERT_CLOSE(inv_mat.x.w, mat.x.w);

		ASSERT_CLOSE(inv_mat.y.x, mat.y.x);
		ASSERT_CLOSE(inv_mat.y.y, mat.y.y);
		ASSERT_CLOSE(inv_mat.y.z, mat.y.z);
		ASSERT_CLOSE(inv_mat.y.w, mat.y.w);

		ASSERT_CLOSE(inv_mat.z.x, mat.z.x);
		ASSERT_CLOSE(inv_mat.z.y, mat.z.y);
		ASSERT_CLOSE(inv_mat.z.z, mat.z.z);
		ASSERT_CLOSE(inv_mat.z.w, mat.z.w);

		ASSERT_CLOSE(inv_mat.w.x, mat.w.x);
		ASSERT_CLOSE(inv_mat.w.y, mat.w.y);
		ASSERT_CLOSE(inv_mat.w.z, mat.w.z);
		ASSERT_CLOSE(inv_mat.w.w, mat.w.w);
	}

	{
		const lak::mat4f_t mat{
		  {1.f, 3.f, 2.f, 1.f},
		  {2.f, 1.f, 3.f, 1.f},
		  {3.f, 2.f, 1.f, 3.f},
		  {2.f, 3.f, 1.f, 2.f},
		};
		const auto inv_mat = inverse(mat);
		ASSERT_CLOSE(inv_mat.x.x, -13.f / 6.f);
		ASSERT_CLOSE(inv_mat.x.y, 6.f / 6.f);
		ASSERT_CLOSE(inv_mat.x.z, -9.f / 6.f);
		ASSERT_CLOSE(inv_mat.x.w, 17.f / 6.f);

		ASSERT_CLOSE(inv_mat.y.x, -1.f / 6.f);
		ASSERT_CLOSE(inv_mat.y.y, 0.f / 6.f);
		ASSERT_CLOSE(inv_mat.y.z, -3.f / 6.f);
		ASSERT_CLOSE(inv_mat.y.w, 5.f / 6.f);

		ASSERT_CLOSE(inv_mat.z.x, 5.f / 6.f);
		ASSERT_CLOSE(inv_mat.z.y, 0.f / 6.f);
		ASSERT_CLOSE(inv_mat.z.z, 3.f / 6.f);
		ASSERT_CLOSE(inv_mat.z.w, -7.f / 6.f);

		ASSERT_CLOSE(inv_mat.w.x, 12.f / 6.f);
		ASSERT_CLOSE(inv_mat.w.y, -6.f / 6.f);
		ASSERT_CLOSE(inv_mat.w.z, 12.f / 6.f);
		ASSERT_CLOSE(inv_mat.w.w, -18.f / 6.f);
	}

	return 0;
}
END_TEST()

BEGIN_TEST(homogeneous_mult)
{
	{
		const lak::mat3f_t mat{
		  {1.f, 0.f, 0.f},
		  {0.f, 1.f, 0.f},
		  {0.f, 0.f, 1.f},
		};
		const lak::vec2f_t vec{2.f, 3.f};
		const auto mult = homogeneous_mult(mat, vec);
		ASSERT_CLOSE(mult.x, 2.f);
		ASSERT_CLOSE(mult.y, 3.f);
	}

	{
		const lak::mat3f_t mat{
		  {1.f, 0.f, 1.f},
		  {0.f, 1.f, 1.f},
		  {0.f, 0.f, 1.f},
		};
		const lak::vec2f_t vec{2.f, 3.f};
		const auto mult = homogeneous_mult(mat, vec);
		ASSERT_CLOSE(mult.x, 3.f);
		ASSERT_CLOSE(mult.y, 4.f);
	}

	{
		const lak::mat4f_t mat{
		  {1.f, 0.f, 0.f, 0.f},
		  {0.f, 1.f, 0.f, 0.f},
		  {0.f, 0.f, 1.f, 0.f},
		  {0.f, 0.f, 0.f, 1.f},
		};
		const lak::vec3f_t vec{1.f, 3.f, 2.f};
		const auto mult = homogeneous_mult(mat, vec);
		ASSERT_CLOSE(mult.x, 1.f);
		ASSERT_CLOSE(mult.y, 3.f);
		ASSERT_CLOSE(mult.z, 2.f);
	}

	{
		const lak::mat4f_t mat{
		  {1.f, 0.f, 0.f, 0.f},
		  {0.f, 1.f, 0.f, 2.f},
		  {0.f, 0.f, 1.f, 1.f},
		  {0.f, 0.f, 0.f, 1.f},
		};
		const lak::vec3f_t vec{1.f, 1.f, 1.f};
		const auto mult = homogeneous_mult(mat, vec);
		ASSERT_CLOSE(mult.x, 1.f);
		ASSERT_CLOSE(mult.y, 3.f);
		ASSERT_CLOSE(mult.z, 2.f);
	}

	return 0;
}
END_TEST()

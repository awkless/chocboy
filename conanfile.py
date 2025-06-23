# SPDX-FileCopyrightText: 2025 Jason Pena <jasonpena@awkless.com>
# SPDX-License-Identifier: CC0-1.0

import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy

class ChocoChip8(ConanFile):
	settings = "os", "compiler", "build_type", "arch"
	generators = "CMakeDeps", "CMakeToolchain"

	def requirements(self):
		self.requires("cxxopts/3.2.0")
		self.requires("fmt/11.2.0")
		self.requires("catch2/3.8.1")
		self.requires("imgui/1.91.8")
		self.requires("sdl/3.2.6")
		self.requires("spdlog/1.15.3")

	def generate(self):
		# INVARIANT: SDL backends for imgui are properly copied into
		# binary directory.
		copy(self, "*sdl*",
		     os.path.join(self.dependencies["imgui"].package_folder,
		                  "res", "bindings"),
       		     os.path.join(self.build_folder, "src/imgui/backends"))

	def layout(self):
		cmake_layout(self)

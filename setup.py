#-*- coding:UTF-8 -*-

from distutils.core import setup, Extension

m_filter_keywords = Extension(
	'filter_keywords',
	include_dirs = ["src"],
	sources = [
		'src/fkw_util.c',
		'filter_keywords.c', 'src/fkw_rbtree.c', 'src/fkw_rbtree_dict.c', 
		'src/mem_collector.c', 'src/fkw_seg.c',
	]
)

setup(
	name = 'filter_keywords',
	version = '0.1',
	author = "Chinaren Wei",
	author_email = "weizhonghua.ati@gmail.com",
	url = "https://pysoft.googlecode.com/svn/trunk/c-project/filter_keywords",
	description = "filter keywords extension",
	license = "GPL",
	ext_modules = [m_filter_keywords],
)

/* +---------------------------------------------------------------------------+
   |          The Mobile Robot Programming Toolkit (MRPT) C++ library          |
   |                                                                           |
   |                   http://mrpt.sourceforge.net/                            |
   |                                                                           |
   |   Copyright (C) 2005-2011  University of Malaga                           |
   |                                                                           |
   |    This software was written by the Machine Perception and Intelligent    |
   |      Robotics Lab, University of Malaga (Spain).                          |
   |    Contact: Jose-Luis Blanco  <jlblanco@ctima.uma.es>                     |
   |                                                                           |
   |  This file is part of the MRPT project.                                   |
   |                                                                           |
   |     MRPT is free software: you can redistribute it and/or modify          |
   |     it under the terms of the GNU General Public License as published by  |
   |     the Free Software Foundation, either version 3 of the License, or     |
   |     (at your option) any later version.                                   |
   |                                                                           |
   |   MRPT is distributed in the hope that it will be useful,                 |
   |     but WITHOUT ANY WARRANTY; without even the implied warranty of        |
   |     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         |
   |     GNU General Public License for more details.                          |
   |                                                                           |
   |     You should have received a copy of the GNU General Public License     |
   |     along with MRPT.  If not, see <http://www.gnu.org/licenses/>.         |
   |                                                                           |
   +---------------------------------------------------------------------------+ */
#ifndef CTextFileLinesParser_H
#define CTextFileLinesParser_H

#include <mrpt/utils/utils_defs.h>
#include <mrpt/system/string_utils.h>

namespace mrpt
{
    namespace utils
    {
			/** A class for parsing text files, returning each non-empty and non-comment line, along its line number.
			  *  Lines are strip out of leading and trailing whitespaces.
			  *  By default, lines starting with either "#", "//" or "%" are skipped (comment lines),
			  *   unless this behavior is explicitly disabled with  \a enableCommentFilters
			  */
			class BASE_IMPEXP CTextFileLinesParser
			{
			public:
				/** Default constructor; should call \a open() at some moment later. */
				CTextFileLinesParser() : m_curLineNum(0), m_filter_MATLAB_comments(true), m_filter_C_comments(true), m_filter_SH_comments(true) { }

				/** Constructor for opening a file  \exception std::exception On error opening file */
				CTextFileLinesParser(const std::string &fil) : m_filter_MATLAB_comments(true), m_filter_C_comments(true), m_filter_SH_comments(true) {
					open(fil);
				}

				/** Open a file (an alternative to the constructor with a file name) */
				void open(const std::string &fil)
				{
					m_curLineNum = 0;
					m_fileName = fil;
					m_in.close();
					m_in.clear(); 
					m_in.open(fil.c_str());
					if (!m_in.is_open())
						THROW_EXCEPTION_CUSTOM_MSG1("Error opening file '%s' for reading",fil.c_str());
				}

				/** Close the file (no need to call it normally, the file is closed upon destruction) */
				void close() { m_in.close(); }

				/** Reset the read pointer to the beginning of the file */
				void rewind()
				{
					m_curLineNum = 0;
					m_in.clear();
					m_in.seekg(0);
				}

				/** Reads from the file and return the next (non-comment) line, as a std::string
				  * \return false on EOF.
				  */
				inline bool getNextLine(std::string &out_str)
				{
					std::istringstream buf;
					if (getNextLine(buf))
					{
						out_str = buf.str();
						return true;
					}
					else
					{
						out_str.clear();
						return false;
					}
				}

				/** Reads from the file and stores the next (non-comment) line into the given stream buffer.
				  * \return false on EOF.
				  */
				bool getNextLine( std::istringstream &buf )
				{
					while (!m_in.fail())
					{
						std::string lin;
						std::getline(m_in,lin);
						m_curLineNum++;
						lin = mrpt::system::trim(lin);
						if (lin.empty()) continue; // Ignore empty lines.
						// Ignore comments lines, starting with "#" or "//".
						if ( (m_filter_SH_comments && mrpt::system::strStarts(lin,"#"))
						  || (m_filter_C_comments  && mrpt::system::strStarts(lin,"//"))
						  || (m_filter_MATLAB_comments && mrpt::system::strStarts(lin,"%")) )
							continue;
						// Parse the line as a string stream:
						buf.str(lin);
						buf.clear();
						return true;
					};
					return false;
				}

				/** Return the line number of the last line returned with \a getNextLine */
				inline size_t getCurrentLineNumber() const { return m_curLineNum; }

				/** Enable/disable filtering of lines starting with "%", "//" or "#", respectively. */
				inline void enableCommentFilters(
					bool filter_MATLAB_comments,
					bool filter_C_comments,
					bool filter_SH_comments
					)
				{
					m_filter_MATLAB_comments = filter_MATLAB_comments;
					m_filter_C_comments = filter_C_comments;
					m_filter_SH_comments = filter_SH_comments;
				}

			private:
				std::string   m_fileName;
				std::ifstream m_in;
				size_t        m_curLineNum;
				bool		  m_filter_MATLAB_comments;
				bool		  m_filter_C_comments;
				bool		  m_filter_SH_comments;

			};  // end of CTextFileLinesParser
	} // End of namespace
} // end of namespace
#endif

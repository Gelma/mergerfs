/*
  Copyright (c) 2019, Antonio SJ Musumeci <trapexit@spawn.link>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "config.hpp"
#include "errno.hpp"
#include "fileinfo.hpp"
#include "fs_base_fchmod.hpp"

#include <fuse.h>

#include <string.h>
#include <sys/stat.h>

namespace l
{
  static
  bool
  is_git(const char   *fusepath_,
         const mode_t  mode_)
  {
    if((mode_ & 0777) != 0444)
      return false;

    return (strstr(fusepath_,"/.git/") != NULL);
  }

  static
  int
  fchmod(const int    fd_,
         const mode_t mode_)
  {
    int rv;

    rv = fs::fchmod(fd_,mode_);
    if(rv == -1)
      return -errno;

    return rv;
  }
}

namespace FUSE
{
  int
  fchmod(const struct fuse_file_info *ffi_,
         const mode_t                 mode_)
  {
    mode_t mode;
    const Config &config = Config::ro();
    FileInfo     *fi     = reinterpret_cast<FileInfo*>(ffi_->fh);

    mode = mode_;
    if((config.git_nfs_hack == true) && l::is_git(fi->fusepath.c_str(),mode_))
      mode |= (S_IWUSR|S_IWGRP);

    return l::fchmod(fi->fd,mode);
  }
}

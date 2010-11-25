/*
   Copyright (c) 2010, The Mineserver Project
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
 * Neither the name of the The Mineserver Project nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "plugin.h"

Plugin &Plugin::get()
{
  static Plugin instance;
  return instance;
}

void Plugin::initPlugin()
{
   // populate all block behaviour with default functions
   this->setBlockBehaviour(BLOCK_STONE, newDelegate(BlockBasic));
   this->setBlockBehaviour(BLOCK_GRASS, newDelegate(BlockBasic));
   this->setBlockBehaviour(BLOCK_DIRT, newDelegate(BlockBasic));
   this->setBlockBehaviour(BLOCK_COBBLESTONE, newDelegate(BlockBasic));   
}

bool Plugin::setBlockBehaviour(int type, std::string func, void* delegate)
{
   if (!blocks.has(type))
   {
      Delegates block;
      blocks.insert(block);
   }
   
   Delegates block = blocks.at(type);
   block.insert(std::pair<std::string,void*>(func, delegate));
   blocks.insert(block);
   return false;
}

bool Plugin::setBlockBehaviour(int type, void* delegate)
{
   this->setBlockBehaviour(type, "onStartedDigging", delegate->onStartedDigging());
   return false;
}
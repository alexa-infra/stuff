local opt = vim.opt
local g = vim.g

g.loaded_node_provider = 0
g.loaded_perl_provider = 0
g.loaded_python3_provider = 0
g.loaded_python_provider = 0
g.loaded_pythonx_provider = 0
g.loaded_ruby_provider = 0

opt.guicursor = ""

opt.nu = true
--vim.opt.relativenumber = true

-- see autocmd.lua
opt.tabstop = 4
opt.softtabstop = 4
opt.shiftwidth = 4
opt.expandtab = true
opt.smartindent = true

opt.wrap = true

opt.swapfile = true
opt.backup = false
opt.undofile = true

-- searching
opt.ignorecase = true -- case insensitive searching
opt.smartcase = true -- case-sensitive if expresson contains a capital letter
opt.hlsearch = true -- highlight search results
opt.incsearch = true -- set incremental search, like modern browsers
opt.lazyredraw = false -- don't redraw while executing macros
opt.magic = true -- set magic on, for regular expressions

opt.termguicolors = true

-- code folding
opt.foldmethod = "expr"
opt.foldexpr = "nvim_treesitter#foldexpr()"
opt.foldlevelstart = 99
opt.foldnestmax = 10
opt.foldenable = false
opt.foldlevel = 1

-- toggle invisible characters
opt.list = true
opt.listchars = {
  tab = "→ ",
  -- eol = "¬",
  trail = "⋅",
  extends = "❯",
  precedes = "❮",
}

 require('nvim-treesitter.configs').setup({
         ensure_installed = { "javascript", "typescript", "python", "lua" },
         highlight = {
                 enable = true,
                 use_languagetree = true,
         },
         indent = { enable = true },
 })

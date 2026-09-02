from pygments.lexer import RegexLexer, bygroups
from pygments.token import Comment, Keyword, Name, Number, Operator, Punctuation, String, Text


class PbbLexer(RegexLexer):
    name = "Lead Build"
    aliases = ["pbb", "lead-build"]
    filenames = ["*.pbb"]

    tokens = {
        "root": [
            (r"#[^\n]*", Comment.Single),
            (r'("(?:\\.|[^"\\])*?")', String.Double),
            (r"\b(let|in|include)\b", Keyword),
            (r"\b(true|false)\b", Keyword.Constant),
            (r"\b\d+(?:\.\d+)?\b", Number),
            (r"\b([a-zA-Z_][a-zA-Z0-9_]*)(\s*)(?=\{)", bygroups(Name.Class, Text)),
            (r"\b([a-zA-Z_][a-zA-Z0-9_]*)(\s*)(?=\()", bygroups(Name.Function, Text)),
            (r"\b(lib\.[a-zA-Z_][a-zA-Z0-9_.]*)\b", Name.Function),
            (r"[{}\[\]();,]", Punctuation),
            (r"[=|]", Operator),
            (r"[+*/!-]", Operator),
            (r"\b[a-zA-Z_][a-zA-Z0-9_]*\b", Name),
            (r"\s+", Text),
            (r".", Text),
        ]
    }

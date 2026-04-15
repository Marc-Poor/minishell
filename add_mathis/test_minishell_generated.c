/* Auto-generated from '800 tests pour minishell.ods' */
#define _POSIX_C_SOURCE 200809L
#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef enum e_expect_mode
{
    EXPECT_SMOKE,
    EXPECT_CONTAINS,
    EXPECT_WILDCARD
}   t_expect_mode;

typedef struct s_case
{
    int             id;
    const char      *input;
    const char      *expected;
    const char      *notes;
    t_expect_mode   mode;
    int             needs_fixture;
}   t_case;

static char *read_all(const char *path)
{
    FILE    *fp;
    long    size;
    char    *buf;

    fp = fopen(path, "rb");
    if (!fp)
        return (NULL);
    if (fseek(fp, 0, SEEK_END) != 0)
        return (fclose(fp), NULL);
    size = ftell(fp);
    if (size < 0)
        return (fclose(fp), NULL);
    rewind(fp);
    buf = calloc((size_t)size + 1, 1);
    if (!buf)
        return (fclose(fp), NULL);
    if (size > 0 && fread(buf, 1, (size_t)size, fp) != (size_t)size)
        return (fclose(fp), free(buf), NULL);
    fclose(fp);
    return (buf);
}

static void strip_ansi(char *s)
{
    size_t  i;
    size_t  j;

    i = 0;
    j = 0;
    while (s[i])
    {
        if ((unsigned char)s[i] == 27 && s[i + 1] == '[')
        {
            i += 2;
            while (s[i] && !isalpha((unsigned char)s[i]))
                i++;
            if (s[i])
                i++;
            continue ;
        }
        s[j++] = s[i++];
    }
    s[j] = '\0';
}

static void collapse_newlines(char *s)
{
    size_t  i;
    size_t  j;

    i = 0;
    j = 0;
    while (s[i])
    {
        if (s[i] != '\n' && s[i] != '\r')
            s[j++] = s[i];
        i++;
    }
    s[j] = '\0';
}

static void trim_prompt_markers(char *s)
{
    char    *p;

    while ((p = strstr(s, "$>")) != NULL)
        memmove(p, p + 2, strlen(p + 2) + 1);
}

static void trim_bash_prefix(char *s)
{
    if (strncmp(s, "bash: ", 6) == 0)
        memmove(s, s + 6, strlen(s + 6) + 1);
}

static char *sanitize_text(const char *src)
{
    char    *dup;

    dup = strdup(src ? src : "");
    if (!dup)
        return (NULL);
    strip_ansi(dup);
    collapse_newlines(dup);
    trim_prompt_markers(dup);
    trim_bash_prefix(dup);
    return (dup);
}

static int match_wildcard(const char *output, const char *expected)
{
    const char  *cursor;
    const char  *next;
    size_t      len;
    char        token[2048];

    cursor = expected;
    while (*cursor)
    {
        next = strstr(cursor, "[...]");
        if (!next)
            return (strstr(output, cursor) != NULL);
        len = (size_t)(next - cursor);
        if (len >= sizeof(token))
            return (0);
        memcpy(token, cursor, len);
        token[len] = '\0';
        if (*token)
        {
            output = strstr(output, token);
            if (!output)
                return (0);
            output += strlen(token);
        }
        cursor = next + 5;
    }
    return (1);
}

static char *run_case(const t_case *tc)
{
    int     fd_in;
    int     fd_out;
    char    in_template[] = "/tmp/minishell_case_in_XXXXXX";
    char    out_template[] = "/tmp/minishell_case_out_XXXXXX";
    char    cmd[1024];
    FILE    *fp;
    char    *output;

    fd_in = mkstemp(in_template);
    fd_out = mkstemp(out_template);
    cr_assert_neq(fd_in, -1, "mkstemp input failed: %s", strerror(errno));
    cr_assert_neq(fd_out, -1, "mkstemp output failed: %s", strerror(errno));
    fp = fdopen(fd_in, "w");
    cr_assert_not_null(fp, "fdopen failed: %s", strerror(errno));
    if (tc->input[0] != '\0')
        fputs(tc->input, fp);
    fputc('\n', fp);
    fputs("exit\n", fp);
    fclose(fp);
    close(fd_out);
    snprintf(cmd, sizeof(cmd), "./minishell < '%s' > '%s' 2>&1", in_template, out_template);
    system(cmd);
    output = read_all(out_template);
    unlink(in_template);
    unlink(out_template);
    cr_assert_not_null(output, "Could not read minishell output for case %d", tc->id);
    return (output);
}

static void assert_case(const t_case *tc)
{
    char    *raw;
    char    *output;
    char    *expected;

    raw = run_case(tc);
    output = sanitize_text(raw);
    expected = sanitize_text(tc->expected);
    free(raw);
    cr_assert_not_null(output);
    cr_assert_not_null(expected);
    if (tc->mode == EXPECT_SMOKE || expected[0] == '\0')
    {
        cr_expect(output != NULL, "Case %d crashed. input=[%s]", tc->id, tc->input);
        free(output);
        free(expected);
        return ;
    }
    if (tc->needs_fixture)
        cr_log_warn("Case %d depends on your local fixture/env. input=[%s] expected=[%s]\n", tc->id, tc->input, tc->expected);
    if (tc->mode == EXPECT_WILDCARD)
        cr_expect(match_wildcard(output, expected),
            "Case %d mismatch\ninput    : %s\nexpected : %s\noutput   : %s",
            tc->id, tc->input, expected, output);
    else
        cr_expect(strstr(output, expected) != NULL,
            "Case %d mismatch\ninput    : %s\nexpected : %s\noutput   : %s",
            tc->id, tc->input, expected, output);
    free(output);
    free(expected);
}

static const t_case g_cases[] =
{
    {1, "", "$>", "", EXPECT_SMOKE, 0},
    {2, "[q", "", "", EXPECT_SMOKE, 0},
    {3, "[q", "", "", EXPECT_SMOKE, 0},
    {4, ":", "", "", EXPECT_SMOKE, 0},
    {5, "!", "", "", EXPECT_SMOKE, 0},
    {6, ">", "bash: syntax error nearunexpected token `newline'$>", "", EXPECT_CONTAINS, 0},
    {7, "<", "", "", EXPECT_SMOKE, 0},
    {8, ">>", "", "", EXPECT_SMOKE, 0},
    {9, "<<", "", "", EXPECT_SMOKE, 0},
    {10, "<>", "", "", EXPECT_SMOKE, 0},
    {11, ">>>>>", "bash: syntax error nearunexpected token `>>'$>", "", EXPECT_CONTAINS, 0},
    {12, ">>>>>>>>>>>>>>>", "bash: syntax error nearunexpected token `>>'$>", "", EXPECT_CONTAINS, 0},
    {13, "<<<<<", "bash: syntax error nearunexpected token `<<'$>", "", EXPECT_CONTAINS, 0},
    {14, "<<<<<<<<<<<<<<<<", "bash: syntax error nearunexpected token `<<'$>", "", EXPECT_CONTAINS, 0},
    {15, "> > > >", "bash: syntax error nearunexpected token `>'$>", "", EXPECT_CONTAINS, 0},
    {16, ">> >> >> >>", "bash: syntax error nearunexpected token `>>'$>", "", EXPECT_CONTAINS, 0},
    {17, ">>>> >> >> >>", "bash: syntax error nearunexpected token `>>'$>", "", EXPECT_CONTAINS, 0},
    {18, "/", "bash: /: Is a directory$>", "", EXPECT_CONTAINS, 0},
    {19, "//", "bash: //: Is a directory$>", "", EXPECT_CONTAINS, 0},
    {20, "/.", "bash: /.: Is a directory$>", "", EXPECT_CONTAINS, 0},
    {21, "/./../../../../..", "bash: /./../../../../..: Is a directory$>", "", EXPECT_CONTAINS, 0},
    {22, "///////", "bash: ///////: Is a directory$>", "", EXPECT_CONTAINS, 0},
    {23, "-", "bash: -: command not found$>", "", EXPECT_CONTAINS, 0},
    {24, "|", "bash: syntax error nearunexpected token `|'$>", "", EXPECT_CONTAINS, 0},
    {25, "| hola", "bash: syntax error nearunexpected token `|'$>", "", EXPECT_CONTAINS, 0},
    {26, "| | |", "bash: syntax error nearunexpected token `|'$>", "", EXPECT_CONTAINS, 0},
    {27, "||", "bash: syntax error nearunexpected token `||'$>", "", EXPECT_CONTAINS, 0},
    {28, "|||||", "bash: syntax error nearunexpected token `||'$>", "", EXPECT_CONTAINS, 0},
    {29, "|||||||||||||", "bash: syntax error nearunexpected token `||'$>", "", EXPECT_CONTAINS, 0},
    {30, ">>|><", "bash: syntax error nearunexpected token `|'$>", "", EXPECT_CONTAINS, 0},
    {31, "&&", "bash: syntax error nearunexpected token `&&'$>", "", EXPECT_CONTAINS, 0},
    {32, "&&&&&", "bash: syntax error nearunexpected token `&&'$>", "", EXPECT_CONTAINS, 0},
    {33, "&&&&&&&&&&&&&&", "bash: syntax error nearunexpected token `&&'$>", "", EXPECT_CONTAINS, 0},
    {34, ";;", "bash: syntax error nearunexpected token `;;'$>", "", EXPECT_CONTAINS, 0},
    {35, ";;;;;", "bash: syntax error nearunexpected token `;;'$>", "", EXPECT_CONTAINS, 0},
    {36, ";;;;;;;;;;;;;;;", "bash: syntax error nearunexpected token `;;'$>", "", EXPECT_CONTAINS, 0},
    {37, "()", "bash: syntax error nearunexpected token `)'$>", "", EXPECT_CONTAINS, 0},
    {38, "( ( ) )", "bash: syntax error nearunexpected token `)'$>", "", EXPECT_CONTAINS, 0},
    {39, "( ( ( ( ) ) ) )", "bash: syntax error nearunexpected token `)'$>", "", EXPECT_CONTAINS, 0},
    {40, "\"\"", "bash: : command not found$>", "", EXPECT_CONTAINS, 0},
    {41, "\"hola\"", "bash: hola: command not found$>", "", EXPECT_CONTAINS, 0},
    {42, "hola'", "bash: hola: command not found$>", "", EXPECT_CONTAINS, 0},
    {43, "'", "bash: : command not found$>", "", EXPECT_CONTAINS, 0},
    {44, "*", "bash: crashtest.c: command not found$>", "", EXPECT_CONTAINS, 1},
    {45, "*/*", "bash: Docs/attest.cation: command not found$>", "", EXPECT_CONTAINS, 1},
    {46, "*/*", "bash: */*: No such file or directory", "", EXPECT_CONTAINS, 0},
    {47, ".", "bash: .: filename argument required$>", "", EXPECT_CONTAINS, 0},
    {48, "..", "bash: ..: command not found$>", "", EXPECT_CONTAINS, 0},
    {49, "~", "bash: /home/vietdu91: Is a directory$>", "", EXPECT_CONTAINS, 1},
    {50, "ABC=hola", "$>", "", EXPECT_SMOKE, 0},
    {51, "4ABC=hola", "bash: 4ABC=hola: command not found$>", "", EXPECT_CONTAINS, 0},
    {52, "hola", "bash: hola: command not found$>", "", EXPECT_CONTAINS, 0},
    {53, "hola que tal", "bash: hola: command not found$>", "", EXPECT_CONTAINS, 0},
    {54, "Makefile", "bash: Makefile: command not found$>", "", EXPECT_CONTAINS, 0},
    {55, "echo", "$>", "", EXPECT_SMOKE, 0},
    {56, "echo -n", "$>", "", EXPECT_SMOKE, 0},
    {57, "echo Hola", "Hola$>", "", EXPECT_CONTAINS, 0},
    {58, "echoHola", "bash: echoHola: command not found$>", "", EXPECT_CONTAINS, 0},
    {59, "echo-nHola", "bash: echo-nHola: command not found$>", "", EXPECT_CONTAINS, 0},
    {60, "echo -n Hola", "Hola$>", "", EXPECT_CONTAINS, 0},
    {61, "echo \"-n\" Hola", "Hola$>", "", EXPECT_CONTAINS, 0},
    {62, "echo -nHola", "-nHola$>", "", EXPECT_CONTAINS, 0},
    {63, "echo Hola -n", "Hola -n$>", "", EXPECT_CONTAINS, 0},
    {64, "echo Hola Que Tal", "Hola Que Tal$>", "", EXPECT_CONTAINS, 0},
    {65, "echo         Hola", "Hola$>", "", EXPECT_CONTAINS, 0},
    {66, "echo    Hola     Que    Tal", "Hola Que Tal$>", "", EXPECT_CONTAINS, 0},
    {67, "echo      \\n hola", "n hola$>", "", EXPECT_CONTAINS, 0},
    {68, "echo \"         \" | cat -e", "         $$>", "", EXPECT_CONTAINS, 0},
    {69, "echo           | cat -e", "$$>", "", EXPECT_CONTAINS, 0},
    {70, "\"\"''echo hola\"\"'''' que\"\"'' tal\"\"''", "hola que tal$>", "", EXPECT_CONTAINS, 0},
    {71, "echo -n -n", "$>", "", EXPECT_SMOKE, 0},
    {72, "echo -n -n Hola Que", "Hola Que$>", "", EXPECT_CONTAINS, 0},
    {73, "echo -p", "-p$>", "", EXPECT_CONTAINS, 0},
    {74, "echo -nnnnn", "$>", "", EXPECT_SMOKE, 0},
    {75, "echo -n -nnn -nnnn", "$>", "", EXPECT_SMOKE, 0},
    {76, "echo -n-nnn -nnnn", "-n-nnn -nnnn$>", "", EXPECT_CONTAINS, 0},
    {77, "echo -n -nnn hola -nnnn", "hola -nnnn$>", "", EXPECT_CONTAINS, 0},
    {78, "echo -n -nnn-nnnn", "-nnn-nnnn$>", "", EXPECT_CONTAINS, 0},
    {79, "echo --------n", "--------n$>", "", EXPECT_CONTAINS, 0},
    {80, "echo -nnn --------n", "--------n$>", "", EXPECT_CONTAINS, 0},
    {81, "echo -nnn -----nn---nnnn", "-----nn---nnnn$>", "", EXPECT_CONTAINS, 0},
    {82, "echo -nnn --------nnnn", "--------nnnn$>", "", EXPECT_CONTAINS, 0},
    {83, "echo $", "$$>", "", EXPECT_CONTAINS, 0},
    {84, "echo $?", "0$>", "", EXPECT_CONTAINS, 0},
    {85, "echo $?$", "0$$>", "", EXPECT_CONTAINS, 0},
    {86, "echo $? | echo $? | echo $?", "0$>", "", EXPECT_CONTAINS, 0},
    {87, "echo $:$= | cat -e", "$:$=$$>", "", EXPECT_CONTAINS, 0},
    {88, "echo \" $ \" | cat -e", " $ $$>", "", EXPECT_CONTAINS, 0},
    {89, "echo ' $ ' | cat -e", " $ $$>", "", EXPECT_CONTAINS, 0},
    {90, "echo $HOME", "/home/vietdu91$>", "", EXPECT_CONTAINS, 1},
    {91, "echo \\$HOME", "$HOME$>", "", EXPECT_CONTAINS, 0},
    {92, "echo my shit terminal is [$TERM]", "my shit terminal is [xterm-256color]$>", "", EXPECT_CONTAINS, 0},
    {93, "echo my shit terminal is [$TERM4", "my shit terminal is [$>", "", EXPECT_CONTAINS, 0},
    {94, "echo my shit terminal is [$TERM4]", "my shit terminal is []$>", "", EXPECT_CONTAINS, 0},
    {95, "echo $UID", "1000$>", "", EXPECT_CONTAINS, 0},
    {96, "echo $HOME9", "$>", "", EXPECT_SMOKE, 0},
    {97, "echo $9HOME", "HOME$>", "", EXPECT_CONTAINS, 0},
    {98, "echo $HOME%", "/home/vietdu91%$>", "", EXPECT_CONTAINS, 1},
    {99, "echo $UID$HOME", "1000/home/vietdu91%$>", "", EXPECT_CONTAINS, 1},
    {100, "echo Le path de mon HOME est $HOME", "Le path de mon HOME est /home/vietdu91$>", "", EXPECT_CONTAINS, 1},
    {101, "echo $USER$var\\$USER$USER\\$USERtest$USER", "vietdu91$USERvietdu91$USERtestvietdu91$>", "", EXPECT_CONTAINS, 1},
    {102, "echo $hola*", "$>", "", EXPECT_SMOKE, 0},
    {103, "echo -nnnn $hola", "$>", "", EXPECT_SMOKE, 0},
    {104, "echo > <", "bash: syntax error near unexpected token `<'$>", "", EXPECT_CONTAINS, 0},
    {105, "echo | |", "bash: syntax error near unexpected token `|'$>", "", EXPECT_CONTAINS, 0},
    {106, "EechoE", "bash: EechoE: command not found$>", "", EXPECT_CONTAINS, 0},
    {107, ".echo.", "bash: .echo.: command not found$>", "", EXPECT_CONTAINS, 0},
    {108, ">echo>", "bash: syntax error near unexpected token `newline'$>", "", EXPECT_CONTAINS, 0},
    {109, "<echo<", "bash: syntax error near unexpected token `newline'$>", "", EXPECT_CONTAINS, 0},
    {110, ">>echo>>", "bash: syntax error near unexpected token `newline'$>", "", EXPECT_CONTAINS, 0},
    {111, "|echo|", "bash: syntax error near unexpected token `|'$>", "", EXPECT_CONTAINS, 0},
    {112, "|echo -n hola", "bash: syntax error near unexpected token `|'$>", "", EXPECT_CONTAINS, 0},
    {113, "echo *", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "", EXPECT_CONTAINS, 1},
    {114, "echo '*'", "*$>", "", EXPECT_CONTAINS, 0},
    {115, "echo D*", "Docs Dockers Drawings$>", "", EXPECT_CONTAINS, 1},
    {116, "echo *Z", "*Z$>", "", EXPECT_CONTAINS, 0},
    {117, "echo *t hola", "crashtest.c test.c hola$>", "", EXPECT_CONTAINS, 1},
    {118, "echo *t", "crashtest.c test.c$>", "", EXPECT_CONTAINS, 1},
    {119, "echo $*", "$>", "", EXPECT_SMOKE, 0},
    {120, "echo hola*hola *", "hola*hola crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "", EXPECT_CONTAINS, 1},
    {121, "echo $hola*", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "", EXPECT_CONTAINS, 1},
    {122, "echo $HOME*", "/home/vietdu91$>", "", EXPECT_CONTAINS, 1},
    {123, "echo $\"\"", "$>", "", EXPECT_SMOKE, 0},
    {124, "echo \"$\"\"\"", "$$>", "", EXPECT_CONTAINS, 0},
    {125, "echo '$'''", "$$>", "", EXPECT_CONTAINS, 0},
    {126, "echo $\"HOME\"", "HOME$>", "", EXPECT_CONTAINS, 0},
    {127, "echo $''HOME", "HOME$>", "", EXPECT_CONTAINS, 0},
    {128, "echo $\"\"HOME", "HOME$>", "", EXPECT_CONTAINS, 0},
    {129, "echo \"$HO\"ME", "ME$>", "", EXPECT_CONTAINS, 0},
    {130, "echo '$HO'ME", "$HOME$>", "", EXPECT_CONTAINS, 0},
    {131, "echo \"$HO\"\"ME\"", "ME$>", "", EXPECT_CONTAINS, 0},
    {132, "echo '$HO''ME'", "$HOME$>", "", EXPECT_CONTAINS, 0},
    {133, "echo \"'$HO''ME'\"", "''ME'$>", "", EXPECT_CONTAINS, 0},
    {134, "echo \"\"$HOME", "/home/vietdu91$>", "", EXPECT_CONTAINS, 1},
    {135, "echo \"\" $HOME", " /home/vietdu91$>", "", EXPECT_CONTAINS, 1},
    {136, "echo ''$HOME", "/home/vietdu91$>", "", EXPECT_CONTAINS, 1},
    {137, "echo '' $HOME", " /home/vietdu91$>", "", EXPECT_CONTAINS, 1},
    {138, "echo $\"HO\"\"ME\"", "HOME$>", "", EXPECT_CONTAINS, 0},
    {139, "echo $'HO''ME'", "HOME$>", "", EXPECT_CONTAINS, 0},
    {140, "echo $'HOME'", "HOME$>", "", EXPECT_CONTAINS, 0},
    {141, "echo \"$\"HOME", "$HOME$>", "", EXPECT_CONTAINS, 0},
    {142, "echo $=HOME", "$=HOME$>", "", EXPECT_CONTAINS, 0},
    {143, "echo $\"HOLA\"", "HOLA$>", "", EXPECT_CONTAINS, 0},
    {144, "echo $'HOLA'", "HOLA$>", "", EXPECT_CONTAINS, 0},
    {145, "echo $DONTEXIST Hola", "Hola$>", "", EXPECT_CONTAINS, 0},
    {146, "echo \"hola\"", "hola$>", "", EXPECT_CONTAINS, 0},
    {147, "echo 'hola'", "hola$>", "", EXPECT_CONTAINS, 0},
    {148, "echo ''hola''", "hola$>", "", EXPECT_CONTAINS, 0},
    {149, "echo ''h'o'la''", "hola$>", "", EXPECT_CONTAINS, 0},
    {150, "echo \"''h'o'la''\"", "'h'o'la''$>", "", EXPECT_CONTAINS, 0},
    {151, "echo \"'\"h'o'la\"'\"", "hola'$>", "", EXPECT_CONTAINS, 0},
    {152, "echo\"'hola'\"", "bash: echo'hola': command not found$>", "", EXPECT_CONTAINS, 0},
    {153, "echo \"'hola'\"", "'hola'$>", "", EXPECT_CONTAINS, 0},
    {154, "echo '\"hola\"'", "\"hola\"$>", "", EXPECT_CONTAINS, 0},
    {155, "echo '''ho\"''''l\"a'''", "ho\"l\"a$>", "", EXPECT_CONTAINS, 0},
    {156, "echo hola\"\"\"\"\"\"\"\"\"\"\"\"", "hola$>", "", EXPECT_CONTAINS, 0},
    {157, "echo hola\"''''''''''\"", "hola''''''''''$>", "", EXPECT_CONTAINS, 0},
    {158, "echo hola''''''''''''", "hola$>", "", EXPECT_CONTAINS, 0},
    {159, "echo hola'\"\"\"\"\"\"\"\"\"\"'", "hola\"\"\"\"\"\"\"\"\"\"$>", "", EXPECT_CONTAINS, 0},
    {160, "e\"cho hola\"", "bash: echo hola: command not found$>", "", EXPECT_CONTAINS, 0},
    {161, "e'cho hola'", "bash: echo hola: command not found$>", "", EXPECT_CONTAINS, 0},
    {162, "echo \"hola     \" | cat -e", "hola     $$>", "", EXPECT_CONTAINS, 0},
    {163, "echo \"\"hola", "hola$>", "", EXPECT_CONTAINS, 0},
    {164, "echo \"\" hola", " hola$>", "", EXPECT_CONTAINS, 0},
    {165, "echo \"\"             hola", " hola$>", "", EXPECT_CONTAINS, 0},
    {166, "echo \"\"hola", "hola$>", "", EXPECT_CONTAINS, 0},
    {167, "echo \"\" hola", " hola$>", "", EXPECT_CONTAINS, 0},
    {168, "echo hola\"\"bonjour", "holabonjour$>", "", EXPECT_CONTAINS, 0},
    {169, "\"e\"'c'ho 'b'\"o\"nj\"o\"'u'r", "bonjour$>", "", EXPECT_CONTAINS, 0},
    {170, "\"\"e\"'c'ho 'b'\"o\"nj\"o\"'u'r\"", "bash: e'c'ho 'b'onjo'u'r: command not found$>", "", EXPECT_CONTAINS, 0},
    {171, "echo \"$DONTEXIST\"Makefile", "Makefile$>", "", EXPECT_CONTAINS, 0},
    {172, "echo \"$DONTEXIST\"\"Makefile\"", "Makefile$>", "", EXPECT_CONTAINS, 0},
    {173, "echo \"$DONTEXIST\" \"Makefile\"", " Makefile$>", "", EXPECT_CONTAINS, 0},
    {174, "$?", "bash: 0: command not found$>", "", EXPECT_CONTAINS, 0},
    {175, "$?$?", "bash: 00: command not found$>", "", EXPECT_CONTAINS, 0},
    {176, "?$HOME", "bash: ?/home/vietdu91: command not found$>", "", EXPECT_CONTAINS, 1},
    {177, "$", "bash: $: command not found$>", "", EXPECT_CONTAINS, 0},
    {178, "$HOME", "bash: /home/vietdu91: Is a directory$>", "", EXPECT_CONTAINS, 1},
    {179, "$HOMEdskjhfkdshfsd", "$>", "", EXPECT_SMOKE, 0},
    {180, "\"$HOMEdskjhfkdshfsd\"", "bash: : command not found$>", "", EXPECT_CONTAINS, 0},
    {181, "$HOMEdskjhfkdshfsd'", "bash: $HOMEsddfddfssfdfs: command not found$>", "", EXPECT_CONTAINS, 0},
    {182, "$DONTEXIST", "$>", "", EXPECT_SMOKE, 0},
    {183, "$LESS$VAR", "bash: -R: command not found$>", "", EXPECT_CONTAINS, 0},
    {184, "Ctlr-C", "^C$>", "", EXPECT_CONTAINS, 0},
    {185, "holaCtlr-C", "hola^C$>", "", EXPECT_CONTAINS, 0},
    {186, "cat (faire Ctlr-C apres avoir fait plusieurs fois [ENTREE])", "^C$>", "", EXPECT_CONTAINS, 0},
    {187, "cat | rev(faire Ctlr-C apres avoir fait plusieurs fois [ENTREE])", "^C$>", "", EXPECT_CONTAINS, 0},
    {188, "Ctlr-D", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {189, "hola Ctlr-D", "hola", "", EXPECT_CONTAINS, 0},
    {190, "Ctlr-\\", "$>", "", EXPECT_SMOKE, 0},
    {191, "hola Ctlr-\\", "hola", "", EXPECT_CONTAINS, 0},
    {192, "cat (faire Ctlr-\\ apres avoir fait plusieurs fois [ENTREE])", "^\\Quit (core dumped)$>", "", EXPECT_CONTAINS, 0},
    {193, "sleep 3 | sleep 3 | sleep 3 (faire Ctlr-C une seconde apres)", "^C$>", "", EXPECT_CONTAINS, 0},
    {194, "sleep 3 | sleep 3 | sleep 3 (faire Ctlr-D une seconde apres)", "$>", "", EXPECT_SMOKE, 0},
    {195, "sleep 3 | sleep 3 | sleep 3 (faire Ctlr-\\ une seconde apres)", "^\\Quit (core dumped)$>", "", EXPECT_CONTAINS, 0},
    {196, "env", "...PATH=...$>", "", EXPECT_CONTAINS, 0},
    {197, "env hola", "env: ʻhola’: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {198, "env hola que tal", "env: ʻhola’: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {199, "env env", "...PATH=...$>", "", EXPECT_CONTAINS, 0},
    {200, "env env env env env", "...PATH=...$>", "", EXPECT_CONTAINS, 0},
    {201, "env ls", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "", EXPECT_CONTAINS, 1},
    {202, "env ./Makefile", "env: ‘./Makefile’: Permission denied$>", "", EXPECT_CONTAINS, 0},
    {203, "export HOLA=bonjourenv", "...HOLA=bonjour$>", "", EXPECT_CONTAINS, 0},
    {204, "export       HOLA=bonjourenv", "...HOLA=bonjour$>", "", EXPECT_CONTAINS, 0},
    {205, "export", "...export PATH=\"...\"$>", "", EXPECT_CONTAINS, 0},
    {206, "export Holaexport", "...export HOME=\"/home/vietdu91\"export Holaexport PATH=\"...\"$>", "", EXPECT_CONTAINS, 1},
    {207, "export Hola9heyexport", "...export HOME=\"/home/vietdu91\"export Hola9heyexport PATH=\"...\"$>", "", EXPECT_CONTAINS, 1},
    {208, "export $DONTEXIST", "...export HOME=\"/home/vietdu91\"export PATH=\"...\"$>", "", EXPECT_CONTAINS, 1},
    {209, "export | grep \"HOME\"", "export HOME=\"/home/vietdu91\"$>", "", EXPECT_CONTAINS, 1},
    {210, "export \"\"", "bash: export: `': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {211, "export =", "bash: export: ʻ=': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {212, "export %", "bash: export: ʻ%': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {213, "export $?", "bash: export: `0': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {214, "export ?=2", "bash: export: ʻ?=2': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {215, "export 9HOLA=", "bash: export: ʻ9HOLA=': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {216, "export HOLA9=bonjourenv", "...HOLA9=bonjour$>", "", EXPECT_CONTAINS, 0},
    {217, "export _HOLA=bonjourenv", "..._HOLA=bonjour$>", "", EXPECT_CONTAINS, 0},
    {218, "export ___HOLA=bonjourenv", "...___HOLA=bonjour$>", "", EXPECT_CONTAINS, 0},
    {219, "export _HO_LA_=bonjourenv", "..._HO_LA_=bonjour$>", "", EXPECT_CONTAINS, 0},
    {220, "export HOL@=bonjour", "bash: export: ʻHOL@=bonjour’: not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {221, "export HOL\\~A=bonjour", "bash: export: `HOL~A=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {222, "export -HOLA=bonjour", "bash: export: -H: invalid option", "", EXPECT_CONTAINS, 0},
    {223, "export --HOLA=bonjour", "bash: export: --: invalid option", "", EXPECT_CONTAINS, 0},
    {224, "export HOLA-=bonjour", "bash: export: `HOLA-=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {225, "export HO-LA=bonjour", "bash: export: `HO-LA=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {226, "export HOL.A=bonjour", "bash: export: `HOL.A=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {227, "export HOL\\\\\\$A=bonjour", "bash: export: `HOL\\$A=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {228, "export HO\\\\\\\\LA=bonjour", "bash: export: `HO\\\\LA=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {229, "export HOL}A=bonjour", "bash: export: `HOL}A=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {230, "export HOL{A=bonjour", "bash: export: `HOL{A=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {231, "export HO*LA=bonjour", "bash: export: `HO*LA=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {232, "export HO#LA=bonjour", "bash: export: `HO#LA=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {233, "export HO@LA=bonjour", "bash: export: `HO@LA=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {234, "export HO!LA=bonjour", "bash: !LA=bonjour: event not found", "", EXPECT_CONTAINS, 0},
    {235, "export HO$?LA=bonjourenv", "...HO0LA=bonjour$>", "", EXPECT_CONTAINS, 0},
    {236, "export +HOLA=bonjour", "bash: export: `+HOLA=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {237, "export HOL+A=bonjour", "bash: export: `HOL+A=bonjour': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {238, "export HOLA+=bonjourenv", "...HOLA=bonjour$>", "", EXPECT_CONTAINS, 0},
    {239, "export HOLA=bonjourexport HOLA+=bonjourenv", "...HOLA=bonjourbonjour$>", "", EXPECT_CONTAINS, 0},
    {240, "exportHOLA=bonjourenv", "...PATH=...$>", "", EXPECT_CONTAINS, 0},
    {241, "export HOLA =bonjour", "bash: export: ʻ=bonjour’: not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {242, "export HOLA = bonjour", "bash: export: ʻ=': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {243, "export HOLA=bon jourenv", "...HOLA=bon$>", "", EXPECT_CONTAINS, 0},
    {244, "export HOLA= bonjourenv", "...HOLA=$>", "", EXPECT_CONTAINS, 0},
    {245, "export HOLA=bonsoirexport HOLA=bonretourexport HOLA=bonjourenv", "...HOLA=bonjour$>", "", EXPECT_CONTAINS, 0},
    {246, "export HOLA=$HOMEenv", "...HOLA=/home/vietdu91$>", "", EXPECT_CONTAINS, 1},
    {247, "export HOLA=bonjour$HOMEenv", "...HOLA=bonjour/home/vietdu91$>", "", EXPECT_CONTAINS, 1},
    {248, "export HOLA=$HOMEbonjourenv", "...HOLA=$>", "", EXPECT_CONTAINS, 0},
    {249, "export HOLA=bon$jourenv", "...HOLA=bon$>", "", EXPECT_CONTAINS, 0},
    {250, "export HOLA=bon\\jourenv", "...HOLA=bonjour$>", "", EXPECT_CONTAINS, 0},
    {251, "export HOLA=bon\\\\jourenv", "...HOLA=bon\\jour$>", "", EXPECT_CONTAINS, 0},
    {252, "export HOLA=bon(jour", "bash: syntax error nearunexpected token `('$>", "", EXPECT_CONTAINS, 0},
    {253, "export HOLA=bon()jour", "bash: syntax error nearunexpected token `('$>", "", EXPECT_CONTAINS, 0},
    {254, "export HOLA=bon&jour", "bash: jour: command not found$>", "", EXPECT_CONTAINS, 0},
    {255, "export HOLA=bon@jourenv", "env...HOLA=bon@jour$>", "", EXPECT_CONTAINS, 0},
    {256, "export HOLA=bon;jourenv", "bash: jour: command not foundenv...HOLA=bon$>", "", EXPECT_CONTAINS, 0},
    {257, "export HOLA=bon!jour", "bash: !jour: event not found$>", "", EXPECT_CONTAINS, 0},
    {258, "export HOLA=bon\"jour\"env", "...HOLA=bonjourPATH=...$>", "", EXPECT_CONTAINS, 0},
    {259, "export HOLA$USER=bonjourenv", "...HOLAvietdu91=bonjourPATH=...$>", "", EXPECT_CONTAINS, 1},
    {260, "export HOLA=bonjour=casse-toiecho $HOLA", "HOLA=bonjour=casse-toi$>", "", EXPECT_CONTAINS, 0},
    {261, "export \"HOLA=bonjour\"=casse-toiecho $HOLA", "HOLA=bonjour=casse-toi$>", "", EXPECT_CONTAINS, 0},
    {262, "export HOLA=bonjourexport BYE=casse-toiecho $HOLA et $BYE", "bonjour et casse-toi$>", "", EXPECT_CONTAINS, 0},
    {263, "export HOLA=bonjour BYE=casse-toiecho $HOLA et $BYE", "bonjour et casse-toi$>", "", EXPECT_CONTAINS, 0},
    {264, "export A=a B=b C=cecho $A $B $C", "", "", EXPECT_SMOKE, 0},
    {265, "export $HOLA=bonjourenv", "...bonjour=bonjourPATH=...$>", "", EXPECT_CONTAINS, 0},
    {266, "export HOLA=\"bonjour      \"  echo $HOLA | cat -e", "bonjour$$>", "", EXPECT_CONTAINS, 0},
    {267, "export HOLA=\"   -n bonjour   \"  echo $HOLA", "bonjour$>", "", EXPECT_CONTAINS, 0},
    {268, "export HOLA=\"bonjour   \"/echo $HOLA", "bonjour /$>", "", EXPECT_CONTAINS, 0},
    {269, "export HOLA='\"'echo \" $HOLA \" | cat -e", " \" $$>", "", EXPECT_CONTAINS, 0},
    {270, "export HOLA=atc$HOLA Makefile", "[cela affiche le Makefile]$>", "", EXPECT_CONTAINS, 0},
    {271, "export \"\" HOLA=bonjourenv", "bash: export: `\": not a valid identifierenv...HOLA=bonjour$>", "", EXPECT_CONTAINS, 0},
    {272, "export HOLA=\"cat Makefile | grep NAME\"  echo $HOLA", "cat Makefile | grep NAME$>", "", EXPECT_CONTAINS, 0},
    {273, "export HOLA=hey echo $HOLA$HOLA$HOLA=hey$HOLA", "heyheyhey=heyhey$>", "", EXPECT_CONTAINS, 0},
    {274, "export HOLA=\"  bonjour  hey  \"  echo $HOLA | cat -e", "bonjour hey$$>", "", EXPECT_CONTAINS, 0},
    {275, "export HOLA=\"  bonjour  hey  \"  echo \"\"\"$HOLA\"\"\" | cat -e", "   bonjour    hey    $$>", "", EXPECT_CONTAINS, 0},
    {276, "export HOLA=\"  bonjour  hey  \"  echo wesh\"$HOLA\" | cat -e", "wesh   bonjour    hey    $$>", "", EXPECT_CONTAINS, 0},
    {277, "export HOLA=\"  bonjour  hey  \"  echo wesh\"\"$HOLA.", "wesh bonjour hey .$>", "", EXPECT_CONTAINS, 0},
    {278, "export HOLA=\"  bonjour  hey  \"  echo wesh$\"\"HOLA.", "weshHOLA.$>", "", EXPECT_CONTAINS, 0},
    {279, "export HOLA=\"  bonjour  hey  \"  echo wesh$\"HOLA HOLA\".", "weshHOLA HOLA.$>", "", EXPECT_CONTAINS, 0},
    {280, "export HOLA=bonjourexport HOLA=\" hola et $HOLA\"echo $HOLA", "hola et bonjour$>", "", EXPECT_CONTAINS, 0},
    {281, "export HOLA=bonjourexport HOLA=' hola et $HOLA'echo $HOLA", "hola et $HOLA$>", "", EXPECT_CONTAINS, 0},
    {282, "export HOLA=bonjourexport HOLA=\" hola et $HOLA\"$HOLAecho $HOLA", "hola et bonjourbonjour$>", "", EXPECT_CONTAINS, 0},
    {283, "export HOLA=\"ls        -l    - a\"echo $HOLA", "ls -l - a$>", "", EXPECT_CONTAINS, 0},
    {284, "export HOLA=\"s -la\" l$HOLA", "total 16drwx------  2 vietdu91 [...] .drwxrwxewt 51 root     [...] ..-rwxr-xr-x  8 vietdu91 [...] crashtest.c[...]", "", EXPECT_WILDCARD, 1},
    {285, "export HOLA=\"s -la\" l\"$HOLA\"", "bash: ls -la: command not found$>", "", EXPECT_CONTAINS, 0},
    {286, "export HOLA=\"s -la\" l'$HOLA'", "bash: l$HOLA: command not found$>", "", EXPECT_CONTAINS, 0},
    {287, "export HOLA=\"l\" $HOLAs", "$>", "", EXPECT_SMOKE, 0},
    {288, "export HOLA=\"l\" \"$HOLA\"s", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "", EXPECT_CONTAINS, 1},
    {289, "export HOL=A=bonjourenv", "...HOL=A=bonjour$>", "", EXPECT_CONTAINS, 0},
    {290, "export HOLA=\"l\" '$HOLA's", "bash: $HOLAs: command not found$>", "", EXPECT_CONTAINS, 0},
    {291, "export HOL=A=\"\"env", "...HOL=A=$>", "", EXPECT_CONTAINS, 0},
    {292, "export TE+S=Tenv", "bash: export: `TE+S=T': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {293, "export \"\"=\"\"", "bash: export: `=': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {294, "export ''=''", "bash: export: `=': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {295, "export \"=\"=\"=\"", "bash: export: `===': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {296, "export '='='='", "bash: export: `===': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {297, "export HOLA=pexport BYE=w$HOLA\"BYE\"d", "bash: pBYEd: command not found$>", "", EXPECT_CONTAINS, 0},
    {298, "export HOLA=pexport BYE=w\"$HOLA\"'$BYE'd", "bash: p$BYEd: command not found$>", "", EXPECT_CONTAINS, 0},
    {299, "export HOLA=pexport BYE=w\"$HOLA\"\"$BYE\"d", "/home/vietdu91/42_works/minishell", "", EXPECT_CONTAINS, 1},
    {300, "export HOLA=pexport BYE=w$\"HOLA\"$\"BYE\"d", "bash: HOLABYEd: command not found$>", "", EXPECT_CONTAINS, 0},
    {301, "export HOLA=pexport BYE=w$'HOLA'$'BYE'd", "bash: HOLABYEd: command not found$>", "", EXPECT_CONTAINS, 0},
    {302, "export HOLA=-n\"echo $HOLA\" hey", "bash: echo -n: command not found$>", "", EXPECT_CONTAINS, 0},
    {303, "export A=1 B=2 C=3 D=4 E=5 F=6 G=7 H=8echo \"$A'$B\"'$C\"$D'$E'\"$F'\"'$G'$H\"", "1'2$C\"$D5\"$F'7'8$>", "", EXPECT_CONTAINS, 0},
    {304, "export HOLA=bonjourenvunset HOLAenv", "...PATH=...$>", "", EXPECT_CONTAINS, 0},
    {305, "export HOLA=bonjourenvunset HOLAunset HOLAenv", "...PATH=...$>", "", EXPECT_CONTAINS, 0},
    {306, "unset PATHecho $PATH", "$>", "", EXPECT_SMOKE, 0},
    {307, "unset PATHls", "bash: ls: No such file or directory", "", EXPECT_CONTAINS, 0},
    {308, "unset \"\"", "bash: unset: `': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {309, "unset INEXISTANT", "$>", "", EXPECT_SMOKE, 0},
    {310, "unset PWDenv | grep PWDpwd", "$>$>/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {311, "pwdunset PWDenv | grep PWDcd $PWDpwd", "/home/vietdu91/42_works/minishell$>$>$>/home/vietdu91", "", EXPECT_CONTAINS, 1},
    {312, "unset OLDPWDenv | grep OLDPWD", "$>$>", "", EXPECT_CONTAINS, 0},
    {313, "unset 9HOLA", "bash: unset: `9HOLA': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {314, "unset HOLA9", "$>", "", EXPECT_SMOKE, 0},
    {315, "unset HOL?A", "bash: unset: `HOL?A': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {316, "unset HOLA HOL?A", "bash: unset: `HOL?A': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {317, "unset HOL?A HOLA", "bash: unset: `HOL?A': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {318, "unset HOL?A HOL.A", "bash: unset: `HOL?A': not a valid identifierbash: unset: `HOL.A': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {319, "unset HOLA=", "bash: unset: `HOLA=': not a valid identifier$>", "", EXPECT_CONTAINS, 0},
    {320, "unset HOL\\\\\\\\A", "bash: unset: `HOL\\\\A': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {321, "unset HOL;A", "bash: A: command not found", "", EXPECT_CONTAINS, 0},
    {322, "unset HOL.A", "bash: unset: `HOL.A': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {323, "unset HOL+A", "bash: unset: `HOL+A': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {324, "unset HOL=A", "bash: unset: `HOL=A': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {325, "unset HOL{A", "bash: unset: `HOL{A': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {326, "unset HOL}A", "bash: unset: `HOL}A': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {327, "unset HOL-A", "bash: unset: `HOL-A': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {328, "unset -HOLA", "bash: unset: -H: invalid option", "", EXPECT_CONTAINS, 0},
    {329, "unset _HOLA", "$>", "", EXPECT_SMOKE, 0},
    {330, "unset HOL_A", "$>", "", EXPECT_SMOKE, 0},
    {331, "unset HOLA_", "$>", "", EXPECT_SMOKE, 0},
    {332, "unset HOL*A", "bash: unset: `HOL*A': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {333, "unset HOL#A", "bash: unset: `HOL#A': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {334, "unset $HOLA", "$>", "", EXPECT_SMOKE, 0},
    {335, "unset $PWD", "bash: unset: `/home/vietdu91/42_works/minishell': not a valid identifier", "", EXPECT_CONTAINS, 1},
    {336, "unset HOL@", "bash: unset: `HOL@': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {337, "unset HOL!A", "bash: !A: event not found", "", EXPECT_CONTAINS, 0},
    {338, "unset HOL^A", "bash: unset: `HOL^A': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {339, "unset HOL$?A", "$>", "", EXPECT_SMOKE, 0},
    {340, "unset HOL\\~A", "bash: unset: `HOL~': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {341, "unset \"\" HOLAenv | grep HOLA", "bash: unset: `': not a valid identifier$>$>", "", EXPECT_CONTAINS, 0},
    {342, "unset PATHecho $PATH", "$>", "", EXPECT_SMOKE, 0},
    {343, "unset PATHcat Makefile", "bash: cat: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {344, "unset =", "bash: unset: `=': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {345, "unset ======", "bash: unset: `======': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {346, "unset ++++++", "bash: unset: `++++++': not a valid identifier", "", EXPECT_CONTAINS, 0},
    {347, "unset _______", "$>", "", EXPECT_SMOKE, 0},
    {348, "unset export", "$>", "", EXPECT_SMOKE, 0},
    {349, "unset echo", "$>", "", EXPECT_SMOKE, 0},
    {350, "unset pwd", "$>", "", EXPECT_SMOKE, 0},
    {351, "unset cd", "$>", "", EXPECT_SMOKE, 0},
    {352, "unset unset", "$>", "", EXPECT_SMOKE, 0},
    {353, "unset sudo", "$>", "", EXPECT_SMOKE, 0},
    {354, "export hola | unset hola | echo $?", "0$>", "", EXPECT_CONTAINS, 0},
    {355, "/bin/echo", "$>", "", EXPECT_SMOKE, 0},
    {356, "/bin/echo Hola Que Tal", "Hola Que Tal$>", "", EXPECT_CONTAINS, 0},
    {357, "/bin/env", "...PATH=...$>", "", EXPECT_CONTAINS, 0},
    {358, "/bin/cd Desktop", "bash: /bin/cd: No such file or directory", "", EXPECT_CONTAINS, 0},
    {359, "history", "...  558 echo $?  559 clear  560 history$>", "", EXPECT_CONTAINS, 0},
    {360, "[t", "clear", "", EXPECT_CONTAINS, 0},
    {361, "pwd", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {362, "pwd hola", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {363, "pwd ./hola", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {364, "pwd hola que tal", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {365, "pwd -p", "bash: pwd: -p: invalid option$>", "", EXPECT_CONTAINS, 1},
    {366, "pwd --p", "bash: pwd: --: invalid option$>", "", EXPECT_CONTAINS, 1},
    {367, "pwd ---p", "bash: pwd: --: invalid option$>", "", EXPECT_CONTAINS, 1},
    {368, "pwd -- p", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {369, "pwd pwd pwd", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {370, "pwd ls", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {371, "pwd ls env", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {372, "cd", "$>", "", EXPECT_SMOKE, 0},
    {373, "cd .", "$>", "", EXPECT_SMOKE, 0},
    {374, "cd ./", "$>", "", EXPECT_SMOKE, 0},
    {375, "cd ./././.", "$>", "", EXPECT_SMOKE, 0},
    {376, "cd ././././", "$>", "", EXPECT_SMOKE, 0},
    {377, "cd ..", "$>", "", EXPECT_SMOKE, 0},
    {378, "cd ../", "$>", "", EXPECT_SMOKE, 0},
    {379, "cd ../..", "$>", "", EXPECT_SMOKE, 0},
    {380, "cd ../.", "$>", "", EXPECT_SMOKE, 0},
    {381, "cd .././././.", "$>", "", EXPECT_SMOKE, 0},
    {382, "cd srcs", "$>", "", EXPECT_SMOKE, 0},
    {383, "cd srcs objs", "bash: cd: too many arguments$>", "", EXPECT_CONTAINS, 0},
    {384, "cd 'srcs'", "$>", "", EXPECT_SMOKE, 0},
    {385, "cd \"srcs\"", "$>", "", EXPECT_SMOKE, 0},
    {386, "cd '/etc'", "$>", "", EXPECT_SMOKE, 0},
    {387, "cd /e'tc'", "$>", "", EXPECT_SMOKE, 0},
    {388, "cd /e\"tc\"", "$>", "", EXPECT_SMOKE, 0},
    {389, "cd sr", "bash: cd: sr: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {390, "cd Makefile", "bash: cd: Makefile: Not a directory$>", "", EXPECT_CONTAINS, 0},
    {391, "cd ../minishell", "$>", "", EXPECT_SMOKE, 0},
    {392, "cd ../../../../../../..", "$>", "", EXPECT_SMOKE, 0},
    {393, "cd .././../.././../bin/ls", "bash: cd: .././../.././../bin/ls: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {394, "cd /", "$>", "", EXPECT_SMOKE, 0},
    {395, "cd '/'", "$>", "", EXPECT_SMOKE, 0},
    {396, "cd //pwd", "//$>", "", EXPECT_CONTAINS, 0},
    {397, "cd '//'pwd", "//$>", "", EXPECT_CONTAINS, 0},
    {398, "cd ///pwd", "/$>", "", EXPECT_CONTAINS, 0},
    {399, "cd ////////pwd", "/$>", "", EXPECT_CONTAINS, 0},
    {400, "cd '////////'pwd", "/$>", "", EXPECT_CONTAINS, 0},
    {401, "cd /minishell", "bash: cd: /minishell: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {402, "cd /cd ..", "$>", "", EXPECT_SMOKE, 0},
    {403, "cd _", "bash: cd: _: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {404, "cd -", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {405, "cd --", "$>", "", EXPECT_SMOKE, 0},
    {406, "cd ---", "bash: cd: --: invalid option$>", "", EXPECT_CONTAINS, 0},
    {407, "cd $HOME", "$>", "", EXPECT_SMOKE, 0},
    {408, "cd $HOME $HOME", "bash: cd: too many arguments$>", "", EXPECT_CONTAINS, 0},
    {409, "cd $HOME/42_works", "$>", "", EXPECT_SMOKE, 0},
    {410, "cd \"$PWD/srcs\"", "$>", "", EXPECT_SMOKE, 0},
    {411, "cd '$PWD/srcs'", "bash: cd: /home/momrane/minishell/src/src: No such file or directory$>", "", EXPECT_CONTAINS, 1},
    {412, "unset HOMEcd $HOME", "bash: cd: HOME not set$>", "", EXPECT_CONTAINS, 0},
    {413, "unset HOMEexport HOME=cd", "$>", "", EXPECT_SMOKE, 0},
    {414, "unset HOMEexport HOMEcd", "bash: cd: HOME not set$>", "", EXPECT_CONTAINS, 0},
    {415, "cd minishell Docs crashtest.c", "bash: cd: too many arguments$>", "", EXPECT_CONTAINS, 0},
    {416, "   cd / | echo $?pwd", "0$>", "", EXPECT_CONTAINS, 0},
    {417, "cd ~", "$>", "", EXPECT_SMOKE, 0},
    {418, "cd ~/", "$>", "", EXPECT_SMOKE, 0},
    {419, "cd ~/ | echo $?pwd", "0$>", "", EXPECT_CONTAINS, 0},
    {420, "cd *", "$>", "", EXPECT_SMOKE, 0},
    {421, "cd *", "bash: cd: minishell: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {422, "cd *", "bash: cd: too many arguments$>", "", EXPECT_CONTAINS, 0},
    {423, "mkdir amkdir a/bcd a/brm -r ../../acd ..", "chdir: error retrieving current directory: getcwd: cannot access parent directories: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {424, "mkdir amkdir a/bcd a/brm -r ../../apwd", "pwd: error retrieving current directory: getcwd: cannot access parent directories: No such file or directory$>", "", EXPECT_CONTAINS, 1},
    {425, "mkdir amkdir a/bcd a/brm -r ../../aecho $PWDecho $OLDPWD", "/home/vietdu91/42_works/minishell/a/b$>/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {426, "mkdir amkdir a/bcd a/brm -r ../../acdecho $PWDech", "/home$>/home/vietdu91/42_works/minishell/a/b$>", "", EXPECT_CONTAINS, 1},
    {427, "mkdir acd arm -r ../aecho $PWDecho $OLDPWD", "../../../home/vietdu91/42_works/minishell/a$>../../../home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {428, "export CDPATH=/cd $HOME/..", "$>", "", EXPECT_SMOKE, 0},
    {429, "export CDPATH=/cd home/vietdu91", "/home/vietdu91$>", "", EXPECT_CONTAINS, 1},
    {430, "export CDPATH=./cd .", "$>", "", EXPECT_SMOKE, 0},
    {431, "export CDPATH=./cd ..", "$>", "", EXPECT_SMOKE, 0},
    {432, "chmod 000 minishell./minishell", "bash: ./minishell : Permission denied$>", "", EXPECT_CONTAINS, 0},
    {433, "ls hola", "ls: cannot access 'hola': No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {434, "./Makefile", "bash: ./Makefile: Permission denied$>", "", EXPECT_CONTAINS, 0},
    {435, "./minishell", "$>", "", EXPECT_SMOKE, 0},
    {436, "env | grep SHLVL./minishellenv | grep SHLVLexitenv", "$>", "", EXPECT_SMOKE, 0},
    {437, "touch hola./hola", "bash: ./hola: Permission denied$>", "", EXPECT_CONTAINS, 0},
    {438, "env|\"wc\" -l", "66$>", "", EXPECT_CONTAINS, 0},
    {439, "env|\"wc \"-l", "bash: wc -l: command not found$>", "", EXPECT_CONTAINS, 0},
    {440, "expr 1 + 1", "2$>", "", EXPECT_CONTAINS, 0},
    {441, "expr $? + $?", "0$>", "", EXPECT_CONTAINS, 0},
    {442, "env -i ./minishellenv", "PWD=/mnt/nfs/homes/emtran/Desktop/42_minishellSHLVL=1_=/usr/bin/env$>", "", EXPECT_CONTAINS, 0},
    {443, "env -i ./minishellexport", "export OLDPWDexport PWD=\"/mnt/nfs/homes/emtran/Desktop/42_minishell\"export SHLVL=\"1\"$>", "", EXPECT_CONTAINS, 0},
    {444, "env -i ./minishellcd", "bash: cd: HOME not set$>", "", EXPECT_CONTAINS, 0},
    {445, "env -i ./minishellcd ~", "$>", "", EXPECT_SMOKE, 0},
    {446, "exit", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {447, "exit exit", "exitbash: exit: exit: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {448, "exit hola", "exitbash: exit: hola: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {449, "exit hola que tal", "exitbash: exit: hola: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {450, "exit 42", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {451, "exit 000042", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {452, "exit 666", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {453, "exit 666 666", "exitbash: exit: too many arguments$>", "", EXPECT_CONTAINS, 0},
    {454, "exit -666 666", "exitbash: exit: too many arguments$>", "", EXPECT_CONTAINS, 0},
    {455, "exit hola 666", "exitbash: exit: hola: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {456, "exit 666 666 666 666", "exitbash: exit: too many arguments$>", "", EXPECT_CONTAINS, 0},
    {457, "exit 666 hola 666", "exitbash: exit: too many arguments", "", EXPECT_CONTAINS, 0},
    {458, "exit hola 666 666", "exitbash: exit: hola: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {459, "exit 259", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {460, "exit -4", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {461, "exit -42", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {462, "exit -0000042", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {463, "exit -259", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {464, "exit -666", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {465, "exit +666", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {466, "exit 0", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {467, "exit +0", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {468, "exit -0", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {469, "exit +42", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {470, "exit -69 -96", "exitbash: exit: too many arguments$>", "", EXPECT_CONTAINS, 0},
    {471, "exit --666", "exitbash: exit: --666: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {472, "exit ++++666", "exitbash: exit: ++++666: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {473, "exit ++++++0", "exitbash: exit: ++++++0: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {474, "exit ------0", "exitbash: exit: ------0: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {475, "exit \"666\"", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {476, "exit '666'", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {477, "exit '-666'", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {478, "exit '+666'", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {479, "exit '----666'", "exitbash: exit: ----666: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {480, "exit '++++666'", "exitbash: exit: ++++666: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {481, "exit '6'66", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {482, "exit '2'66'32'", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {483, "exit \"'666'\"", "exitbash: exit: '666': numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {484, "exit '\"666\"'", "exitbash: exit: \"666\": numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {485, "exit '666'\"666\"666", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {486, "exit +'666'\"666\"666", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {487, "exit -'666'\"666\"666", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {488, "exit 9223372036854775807", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {489, "exit 9223372036854775808", "exitbash: exit: 9223372036854775808: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {490, "exit -9223372036854775808", "exit~/minishell $>", "", EXPECT_CONTAINS, 0},
    {491, "exit -9223372036854775809", "exitbash: exit: -9223372036854775809: numeric argument required~/minishell $>", "", EXPECT_CONTAINS, 0},
    {492, "cat | cat | cat | ls", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "", EXPECT_CONTAINS, 1},
    {493, "ls | exit", "$>", "", EXPECT_SMOKE, 0},
    {494, "ls | exit 42", "$>", "", EXPECT_SMOKE, 0},
    {495, "exit | ls", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "", EXPECT_CONTAINS, 1},
    {496, "echo hola > bonjourexit | cat -e bonjour", "hola$$>", "", EXPECT_CONTAINS, 0},
    {497, "echo hola > bonjourcat -e bonjour | exit", "$>", "", EXPECT_SMOKE, 0},
    {498, "echo | echo", "$>", "", EXPECT_SMOKE, 0},
    {499, "echo hola | echo que tal", "que tal$>", "", EXPECT_CONTAINS, 0},
    {500, "pwd | echo hola", "hola$>", "", EXPECT_CONTAINS, 0},
    {501, "env | echo hola", "hola$>", "", EXPECT_CONTAINS, 0},
    {502, "echo oui | cat -e", "oui$$>", "", EXPECT_CONTAINS, 0},
    {503, "echo oui | echo non | echo hola | grep oui", "$>", "", EXPECT_SMOKE, 0},
    {504, "echo oui | echo non | echo hola | grep non", "$>", "", EXPECT_SMOKE, 0},
    {505, "echo oui | echo non | echo hola | grep hola", "hola$>", "", EXPECT_CONTAINS, 0},
    {506, "echo hola | cat -e | cat -e | cat -e", "hola$$$$>", "", EXPECT_CONTAINS, 0},
    {507, "cd .. | echo \"hola\"", "hola$>", "", EXPECT_CONTAINS, 0},
    {508, "cd / | echo \"hola\"", "hola$>", "", EXPECT_CONTAINS, 0},
    {509, "cd .. | pwd", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {510, "ifconfig | grep \":\"", "docker0: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500   ether 02:42:27:cf:95:49  txqueuelen 0  (Ethernet)[...]$>", "", EXPECT_WILDCARD, 0},
    {511, "ifconfig | grep hola", "$>", "", EXPECT_SMOKE, 0},
    {512, "whoami | grep $USER", "vietdu91$>", "", EXPECT_CONTAINS, 1},
    {513, "whoami | grep $USER > /tmp/bonjourcat /tmp/bonjour", "vietdu91$>", "", EXPECT_CONTAINS, 1},
    {514, "whoami | cat -e | cat -e > /tmp/bonjourcat /tmp/bonjour", "vietdu91$$$>", "", EXPECT_CONTAINS, 1},
    {515, "whereis ls | cat -e | cat -e > /tmp/bonjourcat /tmp/bonjour", "ls: /usr/bin/ls /usr/share/man/man1/ls.1posix.gz /usr/share/man/man1/ls.1.gz$$$>", "", EXPECT_CONTAINS, 0},
    {516, "ls | hola", "bash: hola: command not found$>", "", EXPECT_CONTAINS, 0},
    {517, "ls | ls hola", "ls: cannot access 'hola': No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {518, "ls | ls | hola", "bash: hola: command not found$>", "", EXPECT_CONTAINS, 0},
    {519, "ls | hola | ls", "bash: hola: command not foundcrashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "", EXPECT_CONTAINS, 1},
    {520, "ls | ls | hola | rev", "bash: hola: command not found$>", "", EXPECT_CONTAINS, 0},
    {521, "ls | ls | echo hola | rev", "aloh$>", "", EXPECT_CONTAINS, 0},
    {522, "ls -la | grep \".\"", "total 16drwx------  2 vietdu91 [...] .drwxrwxewt 51 root     [...] ..-rwxr-xr-x  8 vietdu91 [...] crashtest.c[...]", "", EXPECT_WILDCARD, 1},
    {523, "ls -la | grep \"'.'\"", "$>", "", EXPECT_SMOKE, 0},
    {524, "echo test.c | cat -e| cat -e| cat -e| cat -e| cat -e| cat -e| cat -e| cat -e|cat -e|cat -e|cat -e", "test.c$$$$$$$$$$$$>", "", EXPECT_CONTAINS, 1},
    {525, "ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls|ls", "crashtest.c Docs Dockers Drawings Makefile minishell philosophers srcs test.c$>", "", EXPECT_CONTAINS, 1},
    {526, "echo hola | cat | cat | cat | cat | cat | grep hola", "hola$>", "", EXPECT_CONTAINS, 0},
    {527, "echo hola | cat", "hola$>", "", EXPECT_CONTAINS, 0},
    {528, "echo hola| cat", "hola$>", "", EXPECT_CONTAINS, 0},
    {529, "echo hola |cat", "hola$>", "", EXPECT_CONTAINS, 0},
    {530, "echo hola|cat", "hola$>", "", EXPECT_CONTAINS, 0},
    {531, "echo hola || cat", "hola$>", "", EXPECT_CONTAINS, 0},
    {532, "echo hola ||| cat", "bash: syntax error nearunexpected token `|'$>", "", EXPECT_CONTAINS, 0},
    {533, "ech|o hola | cat", "bash: ech: command not foundbash: o: command not found$>", "", EXPECT_CONTAINS, 0},
    {534, "cat Makefile | cat -e | cat -e", "[...]$$.PHONY:    all clean fclean re$$$>", "", EXPECT_WILDCARD, 0},
    {535, "cat Makefile | grep srcs | cat -e", "objs/%.o:                srcs/%.c$                        @norminette srcs/*$$>", "", EXPECT_CONTAINS, 0},
    {536, "cat Makefile | grep srcs | grep srcs | cat -e", "objs/%.o:                srcs/%.c$                        @norminette srcs/*$$>", "", EXPECT_CONTAINS, 0},
    {537, "cat Makefile | grep pr | head -n 5 | cd file_not_exist", "bash: cd: file_not_exist: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {538, "cat Makefile | grep pr | head -n 5 | hello", "bash: hello: command not found$>", "", EXPECT_CONTAINS, 0},
    {539, "export HOLA=bonjour | cat -e | cat -e", "$>", "", EXPECT_SMOKE, 0},
    {540, "unset HOLA | cat -e", "$>", "", EXPECT_SMOKE, 0},
    {541, "export HOLA | echo holaenv | grep PROUT", "hola$>$>", "", EXPECT_CONTAINS, 0},
    {542, "export | echo hola", "hola$>", "", EXPECT_CONTAINS, 0},
    {543, "sleep 3 | sleep 3", "$>", "", EXPECT_SMOKE, 0},
    {544, "time sleep 3 | sleep 3", "real0m3.002suser0m0.003ssys0m0.001s$>", "", EXPECT_CONTAINS, 0},
    {545, "sleep 3 | exit", "$>", "", EXPECT_SMOKE, 0},
    {546, "exit | sleep 3", "$>", "", EXPECT_SMOKE, 0},
    {547, "echo hola > a>>b echo que talcat a | <b cat | cat > c | cat", "$>", "", EXPECT_SMOKE, 0},
    {548, "pwd && ls", "/home/vietdu91/42_works/minishellcrashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {549, "pwd || ls", "/home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {550, "echo hola || echo bonjour", "hola$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {551, "echo hola && echo bonjour", "holabonjour$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {552, "echo bonjour || echo hola", "bonjour$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {553, "echo bonjour && echo hola", "bonjourhola$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {554, "echo -n bonjour && echo -n hola", "bonjourhola$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {555, "pwd && ls && echo hola", "/home/vietdu91/42_works/minishellcrashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.chola$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {556, "pwd || ls && echo hola", "/home/vietdu91/42_works/minishellhola$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {557, "pwd && ls || echo hola", "/home/vietdu91/42_works/minishellcrashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {558, "pwd || ls || echo hola", "/home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {559, "ls || export \"\"", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {560, "export \"\" || ls", "bash: export: `': not a valid identifiercrashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {561, "ls && export \"\"", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.cbash: export: `': not a valid identifier$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {562, "export \"\" && ls", "bash: export: `': not a valid identifier$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {563, "ls || ;", "bash: syntax error nearunexpected token `;'$>", "[BONUS] Marche pour les metacharacters |&;()", EXPECT_CONTAINS, 0},
    {564, "; || ls", "bash: syntax error nearunexpected token `;'$>", "[BONUS] Marche pour les metacharacters |&;()", EXPECT_CONTAINS, 0},
    {565, "ls && ;", "bash: syntax error nearunexpected token `;'$>", "[BONUS] Marche pour les metacharacters |&;()", EXPECT_CONTAINS, 0},
    {566, "; && ls", "bash: syntax error nearunexpected token `;'$>", "[BONUS] Marche pour les metacharacters |&;()", EXPECT_CONTAINS, 0},
    {567, "ls || <", "bash: syntax error near unexpected token `newline' $>", "[BONUS] Marche pour les metacharacters <>", EXPECT_CONTAINS, 0},
    {568, "ls && <", "bash: syntax error near unexpected token `newline' $>", "[BONUS] Marche pour les metacharacters <>", EXPECT_CONTAINS, 0},
    {569, "cat | echo || ls", "$>", "[BONUS]", EXPECT_SMOKE, 0},
    {570, "cat | echo && ls", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {571, "ls || cat | echo", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {572, "ls && cat | echo", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {573, "export \"\" && unset \"\"", "bash: export: `': not a valid identifier", "[BONUS]", EXPECT_CONTAINS, 0},
    {574, "(ls)", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {575, "( ( ls ) )", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {576, "( ( ) ls )", "bash: syntax error near unexpected token `)'$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {577, "ls && (ls)", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.ccrashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {578, "(ls && pwd)", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c/home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {579, "( ( ls&&pwd ) )", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c/home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {580, "( ( ls ) &&pwd )", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c/home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {581, "(ls && ( ( pwd ) ) )", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  srcs  test.c/home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {582, "(ls && pwd) > holacat hola", "crashtest.cDocsDockersDrawingsMakefileminishellphilosopherssrcstest.c/home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {583, "> hola ls && pwd", "/home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {584, "> hola (ls && pwd)", "bash: syntax error near unexpected token `('$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {585, "(> pwd)ls", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  pwd  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {586, "(< pwd)ls", "$>", "[BONUS] Si pwd existe", EXPECT_SMOKE, 0},
    {587, "(< pwd)", "bash: pwd: No such file or directory$>", "[BONUS] Si pwd n'existe pas", EXPECT_CONTAINS, 1},
    {588, "( ( ( ( ( pwd) ) ) ) )", "/home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {589, "() pwd", "bash: syntax error near unexpected token `)'$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {590, "> pwd (ls)", "bash: syntax error near unexpected token `('$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {591, "(ls||pwd)&&(ls||pwd)", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  pwd  srcs  test.ccrashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  pwd  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {592, "(lss||pwd)&&(lss||pwd)", "bash: lss: command not found/home/vietdu91/42_works/minishellbash: lss: command not found/home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {593, "(lss&&pwd)&&(lss&&pwd)", "bash: lss: command not found$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {594, "(ls && pwd | wc) > holacat hola", "crashtest.cDocsDockersDrawingsMakefileminishellphilosopherssrcstest.c      1       1      34$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {595, "(ls && pwd | wc) > hola(ls && pwd | wc) > holacat hola", "crashtest.cDocsDockersDrawingsMakefileminishellphilosopherssrcstest.c      1       1      34$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {596, "(ls && pwd | wc) >> holaecho hey&&(ls && pwd | wc) > holacat hola", "hey$>crashtest.cDocs[...]srcstest.c      1       1      34crashtest.cDocs[...]srcstest.c      1       1      34$>", "[BONUS]", EXPECT_WILDCARD, 1},
    {597, "(pwd | wc) < hola", "      1       1      34$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {598, "(ls && pwd | wc) < hola", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  pwd  srcs  test.c      1       1      34$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {599, "(ls -z || pwd | wc) < hola", "ls: invalid option -- 'z'      1       1      34$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {600, "echo hey > hola(pwd | wc) < hola", "      1       1      34$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {601, "echo hey > hola(ls && pwd | wc) < hola", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  pwd  srcs  test.c      1       1      34$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {602, "echo hey > hola(ls -z || pwd | wc) < hola", "ls: invalid option -- 'z'      1       1      34$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {603, "(ls -z || pwd && ls)", "ls: invalid option -- 'z'/home/vietdu91/42_works/minishellcrashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  pwd  srcs  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {604, "ls || (cat Makefile|grep srcs) && (pwd|wc)", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  pwd  srcs  test.c      1       1      34$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {605, "ls -z && (ls) && (pwd)", "ls: invalid option -- 'z'$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {606, "(ls > Docs/hey && pwd) > holacat holacat Docs/hey", "/home/vietdu91/42_works/minishell$>crashtest.cDocs[...]srcstest.c$>", "[BONUS]", EXPECT_WILDCARD, 1},
    {607, "ls > Docs/hey && pwd > holacat holacat Docs/hey", "/home/vietdu91/42_works/minishell$>crashtest.cDocs[...]srcstest.c$>", "[BONUS]", EXPECT_WILDCARD, 1},
    {608, "cd ../.. && pwd && pwd", "/home/vietdu91//home/vietdu91/$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {609, "(cd ../.. && pwd) && pwd", "/home/vietdu91//home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {610, "ls -z || cd ../../..&&pwdpwd", "ls: invalid option -- 'z'/home/vietdu91$>/home/vietdu91$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {611, "ls -z || (cd ../../..&&pwd)pwd", "ls: invalid option -- 'z'/home/vietdu91$>/home/vietdu91/42_works/minishell$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {612, "echo hola > bonjourcat bonjour", "hola$>", "", EXPECT_CONTAINS, 0},
    {613, "echo que tal >> bonjourcat bonjour", "holaque tal$>", "", EXPECT_CONTAINS, 0},
    {614, "echo hola > bonjourecho que tal >> bonjourcat < bonjour", "$>$>holaque tal$>", "", EXPECT_CONTAINS, 0},
    {615, "echo hola > bonjourrm bonjourecho que tal >> bonjourcat < bonjour", "que tal$>", "", EXPECT_CONTAINS, 0},
    {616, "echo hola que tal > bonjourcat bonjour", "hola que tal$>", "", EXPECT_CONTAINS, 0},
    {617, "echo hola que tal > /tmp/bonjourcat -e /tmp/bonjour", "hola que tal$$>", "", EXPECT_CONTAINS, 0},
    {618, "export HOLA=heyecho bonjour > $HOLAecho $HOLA", "$>hola$>", "", EXPECT_CONTAINS, 0},
    {619, "whereis grep > Docs/bonjourcat Docs/bonjour", "grep: /usr/bin/grep /usr/share/man/man1/grep.1.gz /usr/share/man/man1/grep.1posix.gz /usr/share/info/grep.info.gz$>", "", EXPECT_CONTAINS, 0},
    {620, "ls -la > Docs/bonjourcat Docs/bonjour", "total 16drwx------  2 vietdu91 [...] .drwxrwxewt 51 root     [...] ..-rwxr-xr-x  8 vietdu91 [...] crashtest.c[...]", "", EXPECT_WILDCARD, 1},
    {621, "pwd>bonjourcat bonjour", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {622, "pwd >                     bonjourcat bonjour", "/home/vietdu91/42_works/minishell$>", "", EXPECT_CONTAINS, 1},
    {623, "echo hola > > bonjour", "bash: syntax error near unexpected token `>'$>", "", EXPECT_CONTAINS, 0},
    {624, "echo hola < < bonjour", "bash: syntax error near unexpected token `<'$>", "", EXPECT_CONTAINS, 0},
    {625, "echo hola >>> bonjour", "bash: syntax error near unexpected token `>'$>", "", EXPECT_CONTAINS, 0},
    {626, "> bonjour echo holacat bonjour", "hola$>", "", EXPECT_CONTAINS, 0},
    {627, "> bonjour | echo holacat bonjour", "hola$>$>", "", EXPECT_CONTAINS, 0},
    {628, "prout hola > bonjourls", "bash: prout: command not found$>bonjour [...]", "", EXPECT_WILDCARD, 0},
    {629, "echo hola > hello >> hello >> hellolscat hello", "$>hello$>hola$>", "", EXPECT_CONTAINS, 0},
    {630, "echo hola > hello >> hello >> helloecho hola >> hellocat < hello", "holahola$>", "", EXPECT_CONTAINS, 0},
    {631, "echo hola > hello >> hello >> helloecho hola >> helloecho hola > hello >> hello >> hellocat < hello", "hola$>", "", EXPECT_CONTAINS, 0},
    {632, "echo hola >> hello >> hello > helloecho hola >> hellocat < hello", "holahola$>", "", EXPECT_CONTAINS, 0},
    {633, "echo hola >> hello >> hello > helloecho hola >> helloecho hola >> hello >> hello > hellocat < hello", "hola$>", "", EXPECT_CONTAINS, 0},
    {634, "echo hola > helloecho hola >> hello >> hello >> helloecho hola >> hellocat < hello", "holaholahola$>", "", EXPECT_CONTAINS, 0},
    {635, "echo hola > helloecho hey > bonjourecho <bonjour <hello", "$>", "", EXPECT_SMOKE, 0},
    {636, "echo hola > helloecho hey > bonjourecho <hello <bonjour", "$>", "", EXPECT_SMOKE, 0},
    {637, "echo hola > bonjourecho hey > hellorm bonjour helloecho hola > bonjour > hello > bonjourcat bonjourcat hello", "hola$>$>", "", EXPECT_CONTAINS, 0},
    {638, "echo hola > bonjourecho hey > helloecho hola > bonjour > hello > bonjourcat bonjourcat hello", "hola$>$>", "", EXPECT_CONTAINS, 0},
    {639, "echo hola > bonjourecho hey > hellorm bonjour helloecho hola > bonjour >> hello > bonjourcat bonjourcat hello", "hola$>$>", "", EXPECT_CONTAINS, 0},
    {640, "echo hola > bonjourecho hey > helloecho hola > bonjour > hello > bonjourcat bonjourcat hello", "hola$>$>", "", EXPECT_CONTAINS, 0},
    {641, "echo hola > bonjourecho hey > hellorm bonjour helloecho hola > bonjour > hello >> bonjourcat bonjourcat hello", "hola$>$>", "", EXPECT_CONTAINS, 0},
    {642, "echo hola > bonjourecho hey > helloecho hola > bonjour > hello >> bonjourcat bonjourcat hello", "hola$>$>", "", EXPECT_CONTAINS, 0},
    {643, "echo hola > bonjourecho hey > hellorm bonjour helloecho hola >> bonjour > hello > bonjourcat bonjourcat hello", "hola$>$>", "", EXPECT_CONTAINS, 0},
    {644, "echo hola > bonjourecho hey > helloecho hola >> bonjour > hello > bonjourcat bonjourcat hello", "hola$>$>", "", EXPECT_CONTAINS, 0},
    {645, "echo hola > bonjourecho hey > hellorm bonjour helloecho hola >> bonjour >> hello >> bonjourcat bonjourcat hello", "hola$>$>", "", EXPECT_CONTAINS, 0},
    {646, "echo hola > bonjourecho hey > helloecho hola >> bonjour >> hello >> bonjourcat bonjourcat hello", "holahola$>hey$>", "", EXPECT_CONTAINS, 0},
    {647, "> bonjour echo hola bonjourcat bonjour", "bonjour hola$>", "", EXPECT_CONTAINS, 0},
    {648, ">bonjour echo > hola>bonjour>hola>>bonjour>hola hey >bonjour hola >holacat bonjourcat hola", "$>$>hey hola$>", "", EXPECT_CONTAINS, 0},
    {649, "echo bonjour > hola1echo hello > hola2echo 2 >hola1 >> hola2lscat hola1cat hola2", "hola1  hola2  [...]$>$>hello2$>", "", EXPECT_WILDCARD, 0},
    {650, "echo bonjour > hola1echo hello > hola2echo 2 >>hola1 > hola2lscat hola1cat hola2", "hola1  hola2  [...]$>bonjour$>2$>", "", EXPECT_WILDCARD, 0},
    {651, "> pwdls", "pwd$>", "", EXPECT_CONTAINS, 1},
    {652, "< pwd", "bash: pwd: No such file or directory$>", "", EXPECT_CONTAINS, 1},
    {653, "< Makefile .", ".: not enough arguments$>", "Voir pourquoi", EXPECT_CONTAINS, 0},
    {654, "cat <pwd", "bash: pwd: No such file or directory$>", "", EXPECT_CONTAINS, 1},
    {655, "cat <srcs/pwd", "bash: srcs/pwd: No such file or directory$>", "", EXPECT_CONTAINS, 1},
    {656, "cat <../pwd", "bash: ../pwd: No such file or directory$>", "", EXPECT_CONTAINS, 1},
    {657, "cat >>", "bash: syntax error near unexpected token `newline' $>", "", EXPECT_CONTAINS, 0},
    {658, "cat >>>", "bash: syntax error near unexpected token `>' $>", "", EXPECT_CONTAINS, 0},
    {659, "cat >> <<", "bash: syntax error near unexpected token `<<' $>", "", EXPECT_CONTAINS, 0},
    {660, "cat >> > >> << >>", "bash: syntax error near unexpected token `>' $>", "", EXPECT_CONTAINS, 0},
    {661, "cat < ls", "bash: ls: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {662, "cat < ls > ls", "bash: ls: No such file or directory$>", "", EXPECT_CONTAINS, 0},
    {663, "cat > ls1 < ls2ls", "bash: ls2: No such file or directory$>ls1  [...]$>", "", EXPECT_WILDCARD, 0},
    {664, ">>holacat hola", "$>", "", EXPECT_SMOKE, 0},
    {665, "echo hola > bonjourcat < bonjour", "hola$>", "", EXPECT_CONTAINS, 0},
    {666, "echo hola >bonjourcat <bonjour", "hola$>", "", EXPECT_CONTAINS, 0},
    {667, "echo hola>bonjourcat<bonjour", "hola$>", "", EXPECT_CONTAINS, 0},
    {668, "echo hola> bonjourcat< bonjour", "hola$>", "", EXPECT_CONTAINS, 0},
    {669, "echo hola               >bonjourcat<                     bonjour", "hola$>", "", EXPECT_CONTAINS, 0},
    {670, "echo hola          >     bonjourcat            <         bonjour", "hola$>", "", EXPECT_CONTAINS, 0},
    {671, "echo hola > srcs/bonjourcat < srcs/bonjour", "hola$>", "", EXPECT_CONTAINS, 0},
    {672, "echo hola >srcs/bonjourcat <srcs/bonjour", "hola$>", "", EXPECT_CONTAINS, 0},
    {673, "echo hola > bonjourecho que tal >> bonjourcat < bonjour", "holaque tal$>", "", EXPECT_CONTAINS, 0},
    {674, "echo hola > bonjourrm bonjourecho que tal >> bonjourcat < bonjour", "que tal$>", "", EXPECT_CONTAINS, 0},
    {675, "e'c'\"h\"o hola > bonjourcat 'bo'\"n\"jour", "hola$>", "", EXPECT_CONTAINS, 0},
    {676, "echo hola > bonjour\\ 1lscat bonjour\\ 1", "bonjour 1' [...]$>hola$>", "", EXPECT_WILDCARD, 0},
    {677, "echo hola > bonjour heylscat bonjourcat hey", "bonjour [...]$>hola hey$>cat: hey: No such file or directory$>", "", EXPECT_WILDCARD, 0},
    {678, "echo hola > srcs/bonjourecho hey > srcs/hello>srcs/bonjour >srcs/hello <proutcat srcs/bonjour srcs/hello", "bash: prout: No such file or directory$>$>", "", EXPECT_CONTAINS, 0},
    {679, "echo hola > srcs/bonjourecho hey > srcs/hellorm srcs/bonjour srcs/hello>srcs/bonjour >srcs/hello <proutls srcscat", "bash: prout: No such file or directory$>bonjour  hello$>$>", "", EXPECT_CONTAINS, 0},
    {680, "echo hola > srcs/bonjourecho hey > srcs/hello>srcs/bonjour <prout >srcs/hello cat srcs/bonjour cat srcs/hello", "bash: prout: No such file or directory$>$>hey$>", "", EXPECT_CONTAINS, 0},
    {681, "echo hola > srcs/bonjourecho hey > srcs/hellorm srcs/bonjour srcs/hello>srcs/bonjour <prout >srcs/hello ls srcscat", "bash: prout: No such file or directory$>bonjour$>$>", "", EXPECT_CONTAINS, 0},
    {682, "echo hola > ../bonjourecho hey > ../hello>../bonjour >../hello <proutcat ../bonjour ../hello", "bash: prout: No such file or directory$>$>", "", EXPECT_CONTAINS, 0},
    {683, "echo hola > ../bonjourecho hey > ../hellorm ../bonjour ../hello>../bonjour >../hello <proutls ..cat ../bonjour ../hello", "bash: prout: No such file or directory$>bonjour  hello$>$>", "", EXPECT_CONTAINS, 0},
    {684, "echo hola > ../bonjourecho hey > ../hello>../bonjour <prout >../hello cat ../bonjour cat ../hello", "bash: prout: No such file or directory$>$>hey$>", "", EXPECT_CONTAINS, 0},
    {685, "echo hola > ../bonjourecho hey > ../hellorm ../bonjour ../hello>../bonjour <prout >../hello ls ..cat ../bonjour", "bash: prout: No such file or directory$>bonjour$>$>", "", EXPECT_CONTAINS, 0},
    {686, "echo hola > srcs/bonjourecho hey > srcs/hello>srcs/bonjour >>srcs/hello <proutcat srcs/bonjour cat srcs/hello", "bash: prout: No such file or directory$>$>hey$>", "", EXPECT_CONTAINS, 0},
    {687, "echo hola > srcs/bonjourecho hey > srcs/hello>>srcs/bonjour >srcs/hello <proutcat srcs/bonjour cat srcs/hello", "bash: prout: No such file or directory$>bonjour$>$>", "", EXPECT_CONTAINS, 0},
    {688, "echo hola > srcs/bonjourecho hey > srcs/hello>>srcs/bonjour >>srcs/hello <proutcat srcs/bonjour cat srcs/hello", "bash: prout: No such file or directory$>bonjour$>hey$>", "", EXPECT_CONTAINS, 0},
    {689, "echo hola > srcs/bonjourecho hey > srcs/hello>srcs/bonjour <prout >>srcs/hellocat srcs/bonjour cat srcs/hello", "bash: prout: No such file or directory$>$>hey$>", "", EXPECT_CONTAINS, 0},
    {690, "echo hola > srcs/bonjourecho hey > srcs/hello>>srcs/bonjour <prout >srcs/hellocat srcs/bonjour cat srcs/hello", "bash: prout: No such file or directory$>bonjour$>hey$>", "", EXPECT_CONTAINS, 0},
    {691, "echo hola > srcs/bonjourecho hey > srcs/hello>>srcs/bonjour <prout >>srcs/hellocat srcs/bonjour cat srcs/hello", "bash: prout: No such file or directory$>bonjour$>hey$>", "", EXPECT_CONTAINS, 0},
    {692, "echo hola > srcs/bonjourecho hey > srcs/hello<prout >>srcs/bonjour >>srcs/hellocat srcs/bonjour cat srcs/hello", "bash: prout: No such file or directory$>bonjour$>hey$>", "", EXPECT_CONTAINS, 0},
    {693, "echo hola > bonjourecho hey > hello<bonjour >hellocat bonjour cat hello", "hola$>>>", "", EXPECT_CONTAINS, 0},
    {694, "echo hola > bonjourecho hey > hello>bonjour >hello < proutcat bonjour cat hello", "bash: prout: No such file or directory$>$>$>", "", EXPECT_CONTAINS, 0},
    {695, "echo hola > bonjourecho hey > hellorm bonjour hello>bonjour >hello < proutcat bonjour cat hello", "bash: prout: No such file or directory$>$>$>", "", EXPECT_CONTAINS, 0},
    {696, "echo hola > bonjourecho hey > hello>bonjour <prout hellocat bonjour cat hello", "bash: prout: No such file or directory$>$>hey$>", "", EXPECT_CONTAINS, 0},
    {697, "echo hola > bonjourecho hey > hellorm bonjour hello>bonjour <prout hellocat bonjour ", "bash: prout: No such file or directory$>$>", "", EXPECT_CONTAINS, 0},
    {698, "echo hola > bonjour<bonjour cat | wc > bonjourcat bonjour", "      0       0       0$>", "", EXPECT_CONTAINS, 0},
    {699, "rm -f bonjourrm bonjour > bonjourls -l bonjour", "ls: cannot access 'bonjour': No such file or directory", "", EXPECT_CONTAINS, 0},
    {700, "export HOLA=\"bonjour hello\">$HOLAls", "'bonjour hello' [...]$>", "", EXPECT_WILDCARD, 0},
    {701, "export HOLA=\"bonjour hello\">\"$HOLA\"ls", "'bonjour hello' [...]$>", "", EXPECT_WILDCARD, 0},
    {702, "export HOLA=\"bonjour hello\">$\"HOLA\"ls", "HOLA [...]$>", "", EXPECT_WILDCARD, 0},
    {703, "export HOLA=\"bonjour hello\">$HOLA>heyls", "'bonjour hello' hey [...]$>", "", EXPECT_WILDCARD, 0},
    {704, "export HOLA=\"bonjour hello\">hey>$HOLAls", "'bonjour hello' hey [...]$>", "", EXPECT_WILDCARD, 0},
    {705, "export HOLA=\"bonjour hello\">hey>$HOLA>hey>heyls", "'bonjour hello' hey [...]$>", "", EXPECT_WILDCARD, 0},
    {706, "export A=heyexport A B=Hola D E C=\"Que Tal\"echo $PROUT$B$C > /tmp/a > /tmp/b > /tmp/ccat /tmp/acat /tmp/bcat /tmp/c", "$>$>$>HolaQue Tal$>", "", EXPECT_CONTAINS, 0},
    {707, "                                           !!!!! Contenu du fichier a : \"Amour Tu es Horrible\"                                           !!!!! Contenu du fichier b : \"0123456789\"                                           !!!!! Contenu du fichier c : \"Prout\"", "", "", EXPECT_SMOKE, 0},
    {708, "<a cat <b <c", "Prout$>", "", EXPECT_CONTAINS, 0},
    {709, "<a cat <b <ccat acat bcat c", "$>$>$>Amour Tu es Horrible0123456789Prout$>", "", EXPECT_CONTAINS, 0},
    {710, ">a ls >b >>c >dcat acat bcat ccat d", "$>$>$>Prout$>crashtest.cDocs[...]test.c", "", EXPECT_WILDCARD, 1},
    {711, ">a ls >b >>c >dcat acat bcat ccat d", "$>$>$>Prout$>crashtest.cDocs[...]test.c$>", "", EXPECT_WILDCARD, 1},
    {712, "echo hola > a > b > ccat acat bcat c", "$>$>hola$>", "", EXPECT_CONTAINS, 0},
    {713, "mkdir dirls -la > dir/bonjourcat dir/bonjour", "total 16drwx------  2 vietdu91 [...] .drwxrwxewt 51 root     [...] ..[...]", "", EXPECT_WILDCARD, 1},
    {714, "<acat a", "Amour Tu es Horrible$>", "", EXPECT_CONTAINS, 0},
    {715, ">d cat <a >>ecat acat dcat e", "$>Amour$>Amour$>", "", EXPECT_CONTAINS, 0},
    {716, "< a > b cat > hey >> dcat dls", "Amour Tu Es Horrible$>[...] a b hey d", "", EXPECT_WILDCARD, 0},
    {717, "cat << hola", "> $HOME> hola/home/vietdu91", "", EXPECT_CONTAINS, 1},
    {718, "cat << 'hola'", "> $HOME> hola$HOME$>", "", EXPECT_CONTAINS, 0},
    {719, "cat << \"hola\"", "> $HOME> hola$HOME$>", "", EXPECT_CONTAINS, 0},
    {720, "cat << ho\"la\"", "> $HOME> hola$HOME$>", "", EXPECT_CONTAINS, 0},
    {721, "cat << $HOME", "> prout> /home/vietdu91> $HO> $HOME$>", "", EXPECT_CONTAINS, 1},
    {722, "cat << hola > bonjourcat bonjour", "> prout> croutte> antoine pue> hola$>proutcroutteantoine pue", "", EXPECT_CONTAINS, 0},
    {723, "cat << hola | rev", "> prout> croutte> antoine pue> holatuorpettuorceup eniotna$>", "", EXPECT_CONTAINS, 0},
    {724, "<< hola", "> chola> holache>      hola> \"hola\"> 'hola'> hola$>", "", EXPECT_CONTAINS, 0},
    {725, "<<hola", ">>> hola$>", "", EXPECT_CONTAINS, 0},
    {726, "cat <<", "bash: syntax error near unexpected token `newline'$>", "", EXPECT_CONTAINS, 0},
    {727, "cat << prout << lol << koala", "> prout> lol> koala$>", "", EXPECT_CONTAINS, 0},
    {728, "prout << lol << cat << koala", "> prout> lol> cat> koalabash: prout: command not found", "", EXPECT_CONTAINS, 0},
    {729, "<< $hola", "> $hola$>", "", EXPECT_CONTAINS, 0},
    {730, "<< $\"hola\"$\"b\"", "> holab$>", "", EXPECT_CONTAINS, 0},
    {731, "<< $\"$hola\"$$\"b\"", "> $hola$$b$>", "", EXPECT_CONTAINS, 0},
    {732, "<< ho$la$\"$a\"$$\"b\"", "> hola$a$$b> ho$la$a$$nb$>", "", EXPECT_CONTAINS, 0},
    {733, "echo hola <<< bonjour", "hola$>", "", EXPECT_CONTAINS, 0},
    {734, "echo hola <<<< bonjour", "bash: syntax error near unexpected token `<'$>", "", EXPECT_CONTAINS, 0},
    {735, "echo hola <<<<< bonjour", "bash: syntax error near unexpected token `<<'$>", "", EXPECT_CONTAINS, 0},
    {736, "echo 'yo", "", "", EXPECT_SMOKE, 0},
    {737, "cat <<a >>>out | <<b", "", "", EXPECT_SMOKE, 0},
    {738, "ls *", "crashtest.c  Docs  Dockers  Drawings  Makefile  minishell  philosophers  test.cDocs:identite.pdf  loyer.pdfsrcs:main.c  utils.c  sort.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {739, "ls *.*", "crashtest.c  test.c$>", "[BONUS]", EXPECT_CONTAINS, 1},
    {740, "ls *.hola", "ls: cannot access '*.hola' : No such file or directory$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {741, "cat M*le", "[affiche le Makefile]$>", "[BONUS]", EXPECT_CONTAINS, 0},
    {742, "cat M*ee", "cat: 'M*ee': No such file or directory", "[BONUS]", EXPECT_CONTAINS, 0},
    {743, "cat Make*file", "[affiche le Makefile]$>", "[BONUS]", EXPECT_CONTAINS, 0},
};

Test(minishell_spreadsheet, batch_01)
{
    size_t i = 0;
    while (i < 50)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_02)
{
    size_t i = 50;
    while (i < 100)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_03)
{
    size_t i = 100;
    while (i < 150)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_04)
{
    size_t i = 150;
    while (i < 200)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_05)
{
    size_t i = 200;
    while (i < 250)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_06)
{
    size_t i = 250;
    while (i < 300)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_07)
{
    size_t i = 300;
    while (i < 350)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_08)
{
    size_t i = 350;
    while (i < 400)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_09)
{
    size_t i = 400;
    while (i < 450)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_10)
{
    size_t i = 450;
    while (i < 500)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_11)
{
    size_t i = 500;
    while (i < 550)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_12)
{
    size_t i = 550;
    while (i < 600)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_13)
{
    size_t i = 600;
    while (i < 650)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_14)
{
    size_t i = 650;
    while (i < 700)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

Test(minishell_spreadsheet, batch_15)
{
    size_t i = 700;
    while (i < 743)
    {
        assert_case(&g_cases[i]);
        i++;
    }
}

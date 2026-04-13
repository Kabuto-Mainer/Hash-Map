#include <stdio.h>
#include <assert.h>

#include "type.h"
#include "func.h"

extern const char *KDS_HM_DOT_FILE;

static void kds_hm_create_graph(KDS_HashMapList *list, const char *name) {
    assert(list);
    assert(name);

    FILE *stream = fopen(KDS_HM_DOT_FILE, "wb");
    if (stream == NULL)     ExitF("NULL File", );

    fprintf(stream,
            "digraph {\n"
            "  rankdir=LR;\n"
            "  bgcolor=\"#1e1e1e\"\n"
            "  splines=ortho;\n"
            "  nodesep=0.4;\n"
            "  ranksep=0.6;\n"
            "  node [shape=plaintext, style=filled, fontname=\"Helvetica\"];\n"
            "  edge [arrowhead=vee, arrowsize=0.6, penwidth=1.2];\n\n");

    kds_hm_create_block(list, stream);
    kds_hm_create_line(list, stream);
    fclose(stream);

    char comand[200] = {};
    sprintf(comand,
            "dot %s -T png -o %s",
            KDS_HM_DOT_FILE,
            name);

    int trash = system(comand);
    (void) trash;
    return ;
}

static void kds_hm_create_block(KDS_HashMapList *list, FILE *stream) {
    assert(list);
    assert(stream);

    KDS_Hash hash_cell = list->string ? kds_hm_get_cell_hash(list->string) : 1;
    const char *string = list->string ? list->string : "NULL";

    fprintf(stream,
            "block_%p [label=<\n<TABLE CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
            "<TR><TD PORT=\"root\" BGCOLOR=\"#0308f9ff\" COLSPAN=\"2\"><B>%p</B></TD></TR>\n"
            "<TR><TD BGCOLOR=\"#f46b8bff\" COLSPAN=\"2\">%s</TD></TR>\n"
            "<TR><TD BGCOLOR=\"#05ffff\" COLSPAN=\"2\">%p</TD></TR>\n"
            "<TR><TD BGCOLOR=\"#b209ccff\" COLSPAN=\"2\">" KDS_HASH_PRINT_SPEC "</TD></TR>\n"
            "<TR><TD BGCOLOR=\"#ff6f00\" COLSPAN=\"2\">" KDS_HASH_PRINT_SPEC "</TD></TR>\n"
            "<TR><TD BGCOLOR=\"#08ff3aff\" COLSPAN=\"2\">" KDS_COUNT_PRINT_SPEC "</TD></TR>\n",
            list, list, string, list->next, list->hash_list, hash_cell, list->counter);
    if (list->next != NULL)     kds_hm_create_block(list->next, stream);
}

static void kds_hm_create_line(KDS_HashMapList *list, FILE *stream) {
    assert(list);
    assert(stream);

    if (list->next != NULL) {
        fprintf(stream,
                "block_%p:root -> block_%p:root [color=\"#22ff00\", penwidth = 1.5, arrowsize = 0.6, constraint = true];\n",
                list, list->next);
        kds_hm_create_line(list->next, stream);
    }
    return ;
}

// -------------------------------------------------------------------------------------------------------
/**
 * @brief Функция создания графа по дереву
 * @param [in] tree Указатель на причину дампа
 * @return 0 (если не было ошибок), иначе -> -1
*/
// -------------------------------------------------------------------------------------------------------
int create_graph (tree_t* tree)
{
    assert (tree);

    char address[200] = "";
    make_dir_address (ADDRESS_DUMP_DIR, tree->num_dump, "graph.dot", address);

    FILE* file = fopen (address, "w");
    if (file == NULL)
    {
        EXIT_FUNC("NULL file", -1);
    }

    fprintf (file,
             "digraph {\n"
             "  rankdir=UD;\n"
             "  bgcolor=\"#1e1e1e\"\n"
             "  splines=ortho;\n"
             "  nodesep=0.4;\n"
             "  ranksep=0.6;\n"
             "  node [shape=plaintext, style=filled, fontname=\"Helvetica\"];\n"
             "  edge [arrowhead=vee, arrowsize=0.6, penwidth=1.2];\n\n");

    create_block (tree->null, file);
    create_line (tree->null, file);

    fprintf (file, "\n}\n");

    fclose (file);
    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
/**
 * @brief Функция создания графа для debug
 * @param [in] root Узел, с которого строится граф
 * @return 0 (если не было ошибок), иначе -> -1
 * @note Граф сохраняется в свою директорию ADDRESS_DUMP_DEBUG
*/
// -------------------------------------------------------------------------------------------------------
int debug_graph (node_t* root)
{
    assert (root);

    FILE* file = fopen (ADDRESS_DUMP_DEBUG "graph.dot", "w");
    if (file == NULL)
    {
        EXIT_FUNC("NULL file", -1);
    }

    fprintf (file,
             "digraph {\n"
             "  rankdir=UD;\n"
             "  bgcolor=\"#1e1e1e\"\n"
             "  splines=ortho;\n"
             "  nodesep=0.4;\n"
             "  ranksep=0.6;\n"
             "  node [shape=plaintext, style=filled, fontname=\"Helvetica\"];\n"
             "  edge [arrowhead=vee, arrowsize=0.6, penwidth=1.2];\n\n");

    create_block (root, file);
    create_line (root, file);

    fprintf (file, "\n}\n");

    fclose (file);
    return 0;
}
// -------------------------------------------------------------------------------------------------------



// -------------------------------------------------------------------------------------------------------
/**
 * @brief Рекурсивная функция создания связей между блоками
 * @param [in] node Указатель на текущий узел
 * @param [in] file Указатель на dot файл
 * @return 0
*/
// -------------------------------------------------------------------------------------------------------
int create_line (node_t* node,
                 FILE* file)
{
    assert (node);
    assert (file);

    if (node->left)
    {
        create_line (node->left, file);
    }

    if (node->right)
    {
        create_line (node->right, file);
    }

    if (node->right)
    {
        if (node->right->father == node)
        {
            fprintf (file,
                     "block_%p:right -> block_%p:root [color=\"#e501faff\", penwidth = 1.5, arrowsize = 0.6, constraint = true, dir = both];\n",
                     node, node->right);
        }

        else
        {
            fprintf (file,
                     "block_%p:right -> block_%p:root [color=\"#fa0101ff\", penwidth = 1.5, arrowsize = 0.6, constraint = true];\n",
                     node, node->right);
        }
    }

    if (node->left)
    {
        if (node->left->father == node)
        {
            fprintf (file,
                     "block_%p:left -> block_%p:root [color=\"#e501faff\", penwidth = 1.5, arrowsize = 0.6, constraint = true, dir = both];\n",
                     node, node->left);
        }

        else
        {
            fprintf (file,
                     "block_%p:left -> block_%p:root [color=\"#fa0101ff\", penwidth = 1.5, arrowsize = 0.6, constraint = true];\n",
                     node, node->left);
        }
    }
    return 0;
}
// -------------------------------------------------------------------------------------------------------

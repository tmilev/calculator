  var staticWordCompleter = {
    getCompletions: function(editor, session, pos, prefix, callback) {
        callback(null, AceEditorAutoCompletionWordList.map(function(word) {
            return {
                caption: word,
                value: word,
                meta: "static"
            };
        }));

    }
}

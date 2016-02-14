window.onload = loadProblems();

function loadProblems() {
    $.getJSON("/api/list_problems", function(data) {
        $("#problem-list").empty();
        $.each(data, function(key, val) {
            $("#problem-list").append("<li><a href='/problem/"+val+"'>"+val+"</a></li>");
        });
        $("#problem-list").append(
            "<li><a href='/#create-problem-page' onclick='togglePage(\"create-problem-page\");'>+ Create</a></li>"
        );
    });
}


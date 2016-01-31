$(function() {
    new Morris.Line({
		element: 'problem-performance-area-chart',
		data: [],
        xkey: 'number',
        ykeys: ['performance'],
        labels: ['Performance'],
        pointSize: 2,
        hideHover: 'auto',
        resize: true,
        smooth: false
    });
});

function submitProblem() {
    if($("#submit-problem-btn").hasClass("disabled")) {
        return;
    }

    var problemName = $("#problem-name").val();

    var data = {
        name: problemName
    }

    var lastAlert = $("#submit-problem-form div").last();
    if (lastAlert.hasClass("alert")) {
        lastAlert.remove();
    }

    $.ajax({
        type: "POST",
        url: "/api/submit_problem",
        data: JSON.stringify(data),
        contentType: "application/json; charset=utf-8",
        dataType: "json"
    }).done(
        function(data) {
            $("#submit-problem-form").append(
                "<div class='alert alert-success'><strong>Success!</strong> Problem was created.</div>"
            );
        }
    ).fail(
        function(jqXHR, textStatus, errorThrown) {
            $("#submit-problem-form").append(
                "<div class='alert alert-danger'><strong>Post request failed!</strong> "+jqXHR["responseText"]+"</div>"
            );
            console.log(textStatus);
            console.log(errorThrown);
        }
    );

    loadProblems();
}

window.onload = loadProblems();

function loadProblems() {
    $.getJSON("/api/list_problems", function(data) {
        $("#problem-list").empty();
        $.each(data, function(key, val) {
            $("#problem-list").append("<li><a href='problem/"+val+"'>"+val+"</a></li>");
        });
        $("#problem-list").append(
            "<li><a href='#create-problem-page' onclick='togglePage(\"create-problem-page\");'>+ Create</a></li>"
        );
    });
}


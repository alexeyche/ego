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
    var problemName = $("#problem-name").val();
    

    
    var data = {
        name: problemName,
        D: problemSize
    }

    
    
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


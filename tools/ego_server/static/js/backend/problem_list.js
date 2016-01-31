$.getJSON("api/list_problems", function(data) {
    var items = []
 	$.each(data, function(key, val) {
        items.push(val);
    });
    
    if (items.length == 0) {

    } else {
        $.each(items.reverse(), function(key, val) {
            $("#problem-list").prepend("<li><a href='problem/"+val+"'>"+val+"</a></li>");    
        });
    }
});
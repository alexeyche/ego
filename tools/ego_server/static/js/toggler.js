
function togglePage(pageName) {
    $(".container-fluid").each(function(){
        if($(this).attr('id') == pageName) {
            $(this).parent().removeClass("hidden");
        } else {
            $(this).parent().addClass("hidden");
        }
    })
}

//$('#main-ego-page').parent().toggleClass('hidden');

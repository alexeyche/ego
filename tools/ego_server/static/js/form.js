
$('#int-variable-type').on('click', function() {
    $('#double-variable-type').removeClass('active');
    $('#enum-variable-type').removeClass('active');
    $('#enum-inputs').addClass('hidden');
    $('#min-max-inputs').removeClass('hidden');
});
$('#double-variable-type').on('click', function() {
    $('#int-variable-type').removeClass('active');
    $('#enum-variable-type').removeClass('active');
    $('#enum-inputs').addClass('hidden');
    $('#min-max-inputs').removeClass('hidden');
});
$('#enum-variable-type').on('click', function() {
    $('#int-variable-type').removeClass('active');
    $('#double-variable-type').removeClass('active');
    $('#enum-inputs').removeClass('hidden');
    $('#min-max-inputs').addClass('hidden');
});

function getVariableOptions() {
    if ($('#int-variable-type').hasClass('active')) {
        return "Min: " + $('#variable-min').val() + ", Max: " + $('#variable-max').val();
    } else 
    if ($('#double-variable-type').hasClass('active')) {
        return "Min: " + $('#variable-min').val() + ", Max: " + $('#variable-max').val();
    } else
    if ($('#enum-variable-type').hasClass('active')) {
        return $("#enum-choices").val();
    }
}

function getVariableType() {
    if ($('#int-variable-type').hasClass('active')) {
        return "Int";
    } else
    if ($('#double-variable-type').hasClass('active')) {
        return "Double";
    } else
    if ($('#enum-variable-type').hasClass('active')) {
        return "Enum";
    }
}

$('#add-variable-button').on('click', function() {
    if ($('#variable-name').val().length == 0) {
        $('#variable-name').parent().addClass("has-error");
        return;
    }
    $('#variable-name').parent().removeClass("has-error");
    
    $('#variable-table').find('tbody')
        .append($('<tr>')
            .append($('<td>')
                .text($('#variable-table tr').length)
            )
            .append($('<td>')
                .text($('#variable-name').val())
            )
            .append($('<td>')
                .text(getVariableType())
            )
            .append($('<td>')
                .text(getVariableOptions())
            )
            .append($('<td>')
                .append('<button type="button" class="btn btn-default" id="row-delete-button-'+$('#variable-table tr').length+'">Delete</button>')
                .on('click', function(event) {
                    $(this).parent().remove();
                })
            )
        );
})
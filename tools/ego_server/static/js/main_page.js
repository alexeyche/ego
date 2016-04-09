


$('#int-variable-type').on('click', function() {
    $('#float-variable-type').removeClass('active');
    $('#enum-variable-type').removeClass('active');
    $('#enum-inputs').addClass('hidden');
    $('#min-max-inputs').removeClass('hidden');
});
$('#float-variable-type').on('click', function() {
    $('#int-variable-type').removeClass('active');
    $('#enum-variable-type').removeClass('active');
    $('#enum-inputs').addClass('hidden');
    $('#min-max-inputs').removeClass('hidden');
});
$('#enum-variable-type').on('click', function() {
    $('#int-variable-type').removeClass('active');
    $('#float-variable-type').removeClass('active');
    $('#enum-inputs').removeClass('hidden');
    $('#min-max-inputs').addClass('hidden');
});

function getVariableOptions() {
    if ($('#int-variable-type').hasClass('active')) {
        return "Min: " + $('#variable-min').val() + "; Max: " + $('#variable-max').val();
    } else
    if ($('#float-variable-type').hasClass('active')) {
        return "Min: " + $('#variable-min').val() + "; Max: " + $('#variable-max').val();
    } else
    if ($('#enum-variable-type').hasClass('active')) {
        return $("#enum-choices").val();
    }
}

function getVariableType() {
    if ($('#int-variable-type').hasClass('active')) {
        return "Int";
    } else
    if ($('#float-variable-type').hasClass('active')) {
        return "Float";
    } else
    if ($('#enum-variable-type').hasClass('active')) {
        return "Enum";
    }
}

$('#problem-name').change(function() {
    compileProblemConfig();
})

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
                    if ($('#variable-table tr').length == 0) {
                        $("#submit-problem-btn").prop("disabled", true);
                    }
                    compileProblemConfig();
                })
            )
        );
    $("#submit-problem-btn").prop("disabled", false);
    compileProblemConfig();
})

$('#problem-name').on('keypress', function(e) {
    return e.which !== 13;
});
$('#variable-name').on('keypress', function(e) {
    return e.which !== 13;
});

$("#variable-name").keyup(function(event){
    if(event.keyCode == 13){
        $("#add-variable-button").click();
    }
});




$("#show-problem-config-btn").on('click', function() {
    if (!$(this).hasClass('active')) {
        $(this).addClass('active');
        $("#textarea-problem-config")
            .removeClass("hidden");
        compileProblemConfig();
    } else {
        $(this).removeClass('active');
        $("#textarea-problem-config")
            .addClass("hidden");
    }
})

function compileProblemConfig() {
    var ind = "    ";
    var cfg = "ProblemConfig {\n" +
        ind + "Name: \"" + $("#problem-name").val() + "\"\n"
        ind + "Variable {\n";

    var table = document.getElementById('variable-table');
    for (var i=1; i<table.rows.length; i+=1){
        var row = table.rows[i];

        var varName = row.cells[1].innerHTML;
        var varType = row.cells[2].innerHTML;
        var varOpt = row.cells[3].innerHTML;

        var opts = {}
        var optsArr = varOpt.split(";");
        for (var oi=0; oi < optsArr.length; oi+=1) {
            var optsSpl = optsArr[oi].split(":");

            if (optsSpl.length == 2) {
                opts[optsSpl[0].replace(/ /g,'')] = optsSpl[1].replace(/ /g,'');
            } else {
                optsArr[oi] = optsArr[oi].replace(/ /g,'');
            }
        }

        cfg += ind + "Variable {\n";
        cfg += ind + ind + "Name: \"" + varName + "\"\n";

        if (varType == "Float") {
            cfg += ind + ind + "Type: FLOAT\n";
            cfg += ind + ind + "Min: " + opts["Min"] + "\n";;
            cfg += ind + ind + "Max: " + opts["Max"] + "\n";;
        } else
        if (varType == "Int") {
            cfg += ind + ind + "Type: INT\n";
            cfg += ind + ind + "Min: " + opts["Min"] + "\n";
            cfg += ind + ind + "Max: " + opts["Max"] + "\n";
        } else
        if (varType == "Enum") {
            cfg += ind + ind + "Type: ENUM\n";
            for (var oi=0; oi < optsArr.length; oi += 1) {
                cfg += ind + ind + "Option: \"" + optsArr[oi] + "\"\n";
            }
        }
        cfg += ind + "}\n";
        // $("#cov-model-part-btn > .active")
    }
    cfg += "}\n";
    var acq_pars = $("#acq-pars").val().split(";");
    for (var pid=0; pid < acq_pars.length; ++pid) {
        acq_pars[pid] = ind + ind + ind + "x: " + acq_pars[pid].trim();
    }

    cfg += "SolverConfig {\n";

    cfg += ind + "ModelConfig {\n";
    cfg += ind + ind + "Mean: \"" + $("#mean-model-part-btn .active").text() + "\"\n";
    cfg += ind + ind + "Cov: \"" + $("#cov-model-part-select option:selected").text() + "\"\n";
    cfg += ind + ind + "Lik: \"" + $("#lik-model-part-btn .active").text() + "\"\n";
    cfg += ind + ind + "Inf: \"" + $("#inf-model-part-btn .active").text() + "\"\n";
    cfg += ind + ind + "Acq: \"" + $("#acq-model-part-btn .active").text() + "\"\n";
    cfg += ind + ind + "AcqParameters { \n" + acq_pars.join("\n") + "\n";
    cfg += ind + ind + "}\n";
    cfg += ind + "}\n";
    cfg += ind + "ModelType: \"" + $("#modeltype-model-part-btn .active").text() + "\"\n";
    cfg += ind + "BatchPolicy: \"" + $("#batchpolicy-model-part-btn .active").text() + "\"\n";
    cfg += ind + "BatchSize: " + $("#batch-size").val() + "\n";
    cfg += ind + "InitSampleSize: " + $("#init-sample-size").val() + "\n";
    cfg += "}\n";

    $("#textarea-problem-config").val(cfg);
}

$("#submit-problem-btn").on('click', function() {
    if($("#submit-problem-btn").hasClass("disabled")) {
        return;
    }
    var lastAlert = $("#submit-problem-form div").last();
    if (lastAlert.hasClass("alert")) {
        lastAlert.remove();
    }
    compileProblemConfig();
    $.ajax({
        type: "POST",
        url: "/api/submit_problem",
        data: $("#textarea-problem-config").val(),
        contentType: "text/html; charset=utf-8",
        dataType: "text"
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
})


window.onload = loadModelParts();

function loadModelParts() {
    $.getJSON("/api/list_model_parts", function(data) {
        $.each(data, function(key, val) {
            var btnGrp = $("#"+key.toLowerCase() + "-model-part-btn");
            $.each(val["Values"], function(subKey, subVal) {
                var act = ""
                if (subVal == val["Default"]) {
                    act = "active";
                }
                var name = subVal;
                btnGrp.append(
                    "<button type='button' class='btn btn-default "+act+"' data-toggle='button' id='"+name+"-model-part' onclick='toggleButtonGroup(this.id)'>"+name+"</button>"
                );
            })
            var selGrp = $("#"+key.toLowerCase() + "-model-part-select");
            $.each(val["Values"], function(subKey, subVal) {
                var sel = ""
                if (subVal == val["Default"]) {
                    sel = "selected";
                }
                var name = subVal;
                selGrp.append(
                    "<option id='"+name+"-model-part' "+sel+">" + name + "</option>"
                );
            });
        });
    });
}

function toggleButtonGroup(idButton) {
    var butt = $("#"+idButton);
    butt.parent().children().removeClass("active");
    butt.addClass("active");
}

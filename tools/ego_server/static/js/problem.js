var problemName = window.location.href.split("/").pop();

$("#page-header").text(
    problemName + " problem. "
).append("<small>Statistics overview</small>");


$(document).ready(function(){
    var ajaxPerformanceRenderer = function(url, plot, options) {
        var ret = null;
        $.ajax({
            async: false,
            url: url,
            dataType:"json",
            success: function(data) {
             ret = data;
            }
        });
        var indexedRet = [[]];
        for (var vi=0; vi < ret.length; vi += 1) {
            indexedRet[0].push([vi, ret[vi]]);
        }
        return indexedRet;
    };

    var jsonurl = "/api/problem/"+problemName+ "/performance.json";

    var problemPerformanceChart = $.jqplot('problem-performance-area-chart', jsonurl, {
        dataRenderer: ajaxPerformanceRenderer,
        dataRendererOptions: {
          unusedOptionalUrl: jsonurl
        },
        axes:{
            xaxis: { tickOptions: { formatString:'%d' } },
            yaxis: { tickOptions: { formatString:'%.2f'} }
        },
        highlighter: {
            show: true,
            sizeAdjust: 7.5
        },
        cursor: {
            show: false
        }
    });
});



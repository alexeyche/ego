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
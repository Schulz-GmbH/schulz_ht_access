<template>
	<div class="notifications-container">
		<template v-for="pos in uniquePositions" :key="pos">
			<transition-group appear :name="getTransitionName(pos)" tag="div" class="toasts-wrapper">
				<div v-for="toast in toastsByPosition[pos]" :key="toast.id" class="toast"
					:class="[toast.type, toast.position]" @mouseenter="pause(toast)" @mouseleave="resume(toast)">
					<div class="message mr-4" v-html="toast.message"></div>
					<button class="close-btn" @click="removeToast(toast.id)">✕</button>
				</div>
			</transition-group>
		</template>
	</div>
</template>

<script lang="ts">
export { default } from './notifications'
</script>

<style lang="scss" src="./notifications.scss"></style>
